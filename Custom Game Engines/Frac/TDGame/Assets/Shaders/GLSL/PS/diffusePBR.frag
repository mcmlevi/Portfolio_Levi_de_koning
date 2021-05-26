#version 430 core

#define MAX_POINT_LIGHT_COUNT 18
#define MAX_MAPS 5
#define PI 3.1415926538
#define NON_METAL_SURFACE_REFLECTION 0.04
#define TONE_MAPPING_COEFFICIENT 0.1
#define SHADOW_BIAS 0.0025

struct PointLightVertexData
{
	vec3 Position;
};

struct DirectionalLight
{
    vec3 Color;
    vec3 Direction;
    float Intensity;
};

struct PointLight
{
    vec3 Color;
    float Intensity;
    float Range;
};

struct MaterialData
{
    vec3 AlbedoFactor;
    vec3 EmissionFactor;
    float Metalness;
    float Roughness;

    sampler2D AlbedoTexture;
    sampler2D NormalTexture;
    sampler2D AOMROTexture;
    sampler2D EmissionTexture;
    sampler2D ShadowTexture;
};

uniform PointLight u_PointLights[MAX_POINT_LIGHT_COUNT];
uniform DirectionalLight u_DirectionalLight;
uniform MaterialData u_Material;
uniform bool u_hasAlbedoTexture;
uniform bool u_hasRoughnessMetalnessTexture;
uniform bool u_HasEmissiveTexture;
uniform bool u_HasDirectionalLight;
uniform int u_PointLightCount;
uniform bool u_UseBakedAmbientOcclusion;
uniform bool u_FlipNormals;

out vec4 FragColor;

in VS_OUT
{
	vec2 TexCoords;
	vec3 TS_Position;
	vec3 TS_ViewDir;
	vec3 TS_DirectionalLightDir;
	vec3 TS_PointLightPositions[MAX_POINT_LIGHT_COUNT];
    vec4 LS_Position;
} IN;

float AmbientIntensity = 0.1;

vec3 CalculateCookTorranceBRDF(vec3 viewDir, vec3 lightDir, vec3 lightRadiance, vec3 surfReflection, vec3 normal, float metallic, float roughness);

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 surfReflection);
float ShadowCalculation(vec4 fragPosLightSpace);

float CalculateLightAttenuation(in vec3 lightPosition, float intensity, float range);

void main()
{
    vec3 viewDir = normalize(IN.TS_ViewDir);
    vec3 albedo = u_hasAlbedoTexture 
        ? texture(u_Material.AlbedoTexture, IN.TexCoords).rgb
        : u_Material.AlbedoFactor;

    vec3 normal = normalize(texture(u_Material.NormalTexture, IN.TexCoords).rgb * 2.0f - 1.0f);
    if(u_FlipNormals) 
    {
        normal.y *= -1;
    }
    float ambientOcclusion,roughness,metallic;

    if(u_hasRoughnessMetalnessTexture)
    {
        vec3 PBRValue = texture(u_Material.AOMROTexture, IN.TexCoords).rgb;
        ambientOcclusion = PBRValue.r;
        roughness = PBRValue.g;
        metallic = PBRValue.b;
    }
    else
    {
        ambientOcclusion = 1;
        roughness = u_Material.Roughness;
        metallic = u_Material.Metalness;
    }

    vec3 surfaceReflection = vec3(0.04);
    surfaceReflection = mix(surfaceReflection, albedo, metallic);
    vec3 lo = vec3(0.0);

    //Directional light.
    float shadow = ShadowCalculation(IN.LS_Position);
    if(u_HasDirectionalLight)
    {
        vec3 lightDir = normalize(IN.TS_DirectionalLightDir);
        vec3 lightRadiance = u_DirectionalLight.Color * u_DirectionalLight.Intensity;
        lo += CalculateCookTorranceBRDF(viewDir, lightDir, lightRadiance, surfaceReflection, normal, metallic, roughness) * (1.0 - shadow);
    }

    //Point lights.
    for(int i = 0; i < u_PointLightCount; i++)
    {
        vec3 lightDir = normalize(IN.TS_PointLightPositions[i] - IN.TS_Position);
        float lightAttenuation = CalculateLightAttenuation(IN.TS_PointLightPositions[i], u_PointLights[i].Intensity, u_PointLights[i].Range);
        vec3 lightRadiance = u_PointLights[i].Color * lightAttenuation;
        lo += CalculateCookTorranceBRDF(viewDir, lightDir, lightRadiance, surfaceReflection, normal, metallic, roughness);
    }

    //Emission.
    vec3 emission = vec3(0);
    if(u_HasEmissiveTexture) 
    {
        emission = texture(u_Material.EmissionTexture, IN.TexCoords).rgb * u_Material.EmissionFactor;
    }

    //AO.
    vec3 ambient = u_UseBakedAmbientOcclusion 
        ? vec3(AmbientIntensity * albedo * ambientOcclusion* (1.0 - shadow))
        : vec3(AmbientIntensity * albedo* (1.0 - shadow));

    //Gamma correction. (hardcoded needs to be moved to a new renderpass.
    vec3 srgb = emission + ambient + lo;
    vec3 mapped = vec3(1.0) - exp(-srgb * TONE_MAPPING_COEFFICIENT);
    vec3 rgb = pow(mapped, vec3(1.0 / 2.2));
    FragColor = vec4(rgb, 1.0);
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projectionCoords = (fragPosLightSpace.xyz / fragPosLightSpace.w) * 0.5 + 0.5;
    float closestDepth = texture(u_Material.ShadowTexture, projectionCoords.xy).r;
    float currentDepth = projectionCoords.z;

    if(projectionCoords.z > 1.0f)
    {
        return 0.0f;
    }

    return (currentDepth - SHADOW_BIAS > closestDepth) 
        ? 1.0f 
        : 0.0f;
}


//Based on https://learnopengl.com/PBR/Lighting and changed around for better results.
vec3 CalculateCookTorranceBRDF(vec3 viewDir, vec3 lightDir, vec3 lightRadiance, vec3 surfReflection, vec3 normal, float metallic, float roughness)
{
    vec3 albedo;
    if(u_hasAlbedoTexture){
        albedo = texture(u_Material.AlbedoTexture, IN.TexCoords).rgb;
    }else{
        albedo = u_Material.AlbedoFactor;
    }
    vec3 halfway = normalize(viewDir + lightDir);

    float nDotV = abs(dot(normal, viewDir));
    float nDotL = abs(dot(normal, lightDir));
    float hDotV = abs(dot(halfway, viewDir));

    //Calculate the NDF and the Geometry function for the cook torrance BRDF.
    float ndf = DistributionGGX(normal, halfway, roughness);
    float g = GeometrySmith(normal, viewDir, lightDir, roughness);
    vec3 f = FresnelSchlick(hDotV, surfReflection);

    //Calculate the light's contribution to the reflectance equation - which is what fresnel represents.
    vec3 reflectedEnergy = f;
    vec3 absorbedEnergy = vec3(1.0) - reflectedEnergy;
    absorbedEnergy *= 1.0 - metallic;

    //Calculate the diffuse and specular component.
    vec3 diffuse = absorbedEnergy * albedo / PI;
    vec3 numerator = ndf * g * f;
    float denominator = 4.0 * nDotV * nDotL;
    vec3 specular = numerator / max(denominator, 0.001);

    return (diffuse + specular) * lightRadiance * nDotL;
}

vec3 FresnelSchlick(float cosTheta, vec3 surfReflection)
{
    float c = 1.0 - cosTheta;
    return surfReflection + (1.0 - surfReflection) * (c * c * c * c * c); //don't sure POW, it's inefficient.
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

float CalculateLightAttenuation(in vec3 lightPosition, float intensity, float range)
{
    float distance = length(lightPosition - IN.TS_Position);

    float E = intensity;
    float DD = range * range;
    float Q = 1;
    float rr = distance * distance;

    return intensity * (DD / (DD + Q * rr));
}
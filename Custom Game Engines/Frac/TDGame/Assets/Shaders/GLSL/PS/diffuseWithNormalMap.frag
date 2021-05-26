#version 430 core

#define MAX_POINT_LIGHT_COUNT 4
#define MAX_MAPS 5

struct DirectionalLight
{
    vec3 Color;
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

    sampler2D EmissionTexture;
    sampler2D AlbedoTexture;
    sampler2D NormalTexture;
};

uniform PointLight u_PointLights[MAX_POINT_LIGHT_COUNT];
uniform DirectionalLight u_DirectionalLight;
uniform MaterialData u_Material;
uniform bool u_HasDirectionalLight;
uniform bool u_HasEmissiveTexture;
uniform int u_PointLightCount;
uniform bool u_FlipNormals;

out vec4 FragColor;

in VS_OUT
{
	vec2 TexCoords;

	vec3 TS_Position;
	vec3 TS_ViewDir;
	vec3 TS_DirectionalLightDir;
	vec3 TS_PointLightPositions[MAX_POINT_LIGHT_COUNT];
} IN;

vec3 SpecularLightColor = vec3(0.5, 0.5, 0.5);
float AmbientIntensity = 0.03;

float CalculateLightAttenuation(in vec3 lightPosition, in float intensity);
vec3 CalculateDirectionalLight(in vec3 viewDir, in vec3 lightPos, in vec3 normal);
vec3 CalculatePointLight(in PointLight light, in vec3 lightPos, in vec3 normal, in vec3 viewDir);

void main()
{
    vec3 normal = normalize(texture(u_Material.NormalTexture, IN.TexCoords).rgb * 2.0f - 1.0f);
    if(u_FlipNormals) normal.y *= -1;
    vec3 color = u_HasDirectionalLight
        ? CalculateDirectionalLight(IN.TS_DirectionalLightDir, IN.TS_ViewDir, normal) 
        : vec3(0, 0, 0);

    for(int i = 0; i < u_PointLightCount; i++)
    {
        color += CalculatePointLight(u_PointLights[i], IN.TS_PointLightPositions[i], normal, IN.TS_ViewDir);
    }

    vec3 texColor = texture(u_Material.AlbedoTexture, IN.TexCoords).rgb;
    vec3 ambient = AmbientIntensity * texColor;
    vec3 emission = vec3(0);
    if(u_HasEmissiveTexture) emission = texture(u_Material.EmissionTexture, IN.TexCoords).rgb * u_Material.EmissionFactor;
    FragColor = vec4(ambient + emission + color, 1.0);
}

vec3 CalculateDirectionalLight(in vec3 lightDir, in vec3 viewDir, in vec3 normal)
{
    vec3 halfAngle = normalize(viewDir + lightDir);

    vec4 TexColor = texture(u_Material.AlbedoTexture, IN.TexCoords);
    if(TexColor.a < 0.5)
        discard;

    vec3 diffuse = max(dot(normal, lightDir), 0.0) * TexColor.rgb * u_DirectionalLight.Color.rgb;
    vec3 specular = SpecularLightColor * pow(max(0.0, dot(normal, halfAngle)), 100);
    
    return (diffuse + specular);
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

vec3 CalculatePointLight(in PointLight light, in vec3 lightPos, in vec3 normal, in vec3 viewDir)
{
    vec3 lightDir = normalize(lightPos - IN.TS_Position);
    vec3 halfAngle = normalize(viewDir + lightDir);

    vec4 TexColor = texture(u_Material.AlbedoTexture, IN.TexCoords);
    if(TexColor.a < 0.5)
        discard;

    vec3 diffuse = max(0.0, dot(normal, lightDir)) * light.Color * TexColor.rgb;
    vec3 specular = pow(max(0.0, dot(normal, halfAngle)), 1000) * SpecularLightColor;
    
    return CalculateLightAttenuation(lightPos, light.Intensity, light.Range) * diffuse + specular;
}
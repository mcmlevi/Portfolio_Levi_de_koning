#version 430 core

#define MAX_POINT_LIGHT_COUNT 4
#define MAX_MAPS 5

struct DirectionalLight
{
    vec3 Color;
    vec3 Direction;
    float Intensity;
};

struct PointLight
{
    vec3 Position;
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
};

uniform PointLight u_PointLights[MAX_POINT_LIGHT_COUNT];
uniform DirectionalLight u_DirectionalLight;
uniform MaterialData u_Material;
uniform bool u_HasEmissiveTexture;
uniform bool u_HasDirectionalLight;
uniform int u_PointLightCount;
uniform vec3 u_CameraPos;

out vec4 FragColor;

in VS_OUT
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
} IN;

vec3 SpecularLightColor = vec3(0.5, 0.5, 0.5);
float AmbientIntensity = 0.2;

float CalculateLightAttenuation(in vec3 lightPosition, in float intensity);
vec3 CalculateDirectionalLight(in vec3 viewDir, in vec3 lightPos, in vec3 normal);
vec3 CalculatePointLight(in PointLight light, in vec3 lightPos, in vec3 normal, in vec3 viewDir);

void main()
{
    vec4 texColor = texture(u_Material.AlbedoTexture, IN.TexCoords);
    vec3 toEye = normalize(u_CameraPos - IN.Position);
    vec3 color = u_HasDirectionalLight
        ? CalculateDirectionalLight(u_DirectionalLight.Direction, toEye, IN.Normal) 
        : vec3(0, 0, 0);

    for(int i = 0; i < u_PointLightCount; i++)
    {
        color += CalculatePointLight(u_PointLights[i], u_PointLights[i].Position, IN.Normal, toEye);
    }

    vec3 ambient = texColor.rgb * AmbientIntensity;
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

    vec3 ambient = AmbientIntensity * TexColor.rgb;
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * TexColor.rgb * u_DirectionalLight.Color;
    vec3 specular = SpecularLightColor * pow(max(0.0, dot(normal, halfAngle)), 1000);
    
    return (diffuse + specular);
}

float CalculateLightAttenuation(in vec3 lightPosition, float intensity, float range)
{
    float distance = length(lightPosition - IN.Position);

    float E = intensity;
    float DD = range * range;
    float Q = 1;
    float rr = distance * distance;

    return intensity * (DD / (DD + Q * rr));
}

vec3 CalculatePointLight(in PointLight light, in vec3 lightPos, in vec3 normal, in vec3 viewDir)
{
    vec3 lightDir = normalize(lightPos - IN.Position);
    vec3 halfAngle = normalize(viewDir + lightDir);

    vec4 TexColor = texture(u_Material.AlbedoTexture, IN.TexCoords);
    if(TexColor.a < 0.5)
        discard;

    vec3 diffuse = max(dot(normal, lightDir), 0.0) * TexColor.rgb * light.Color;
    vec3 specular = pow(max(dot(normal, halfAngle), 0.0), 500) * SpecularLightColor;
    
    return CalculateLightAttenuation(lightPos, light.Intensity, light.Range) * diffuse + specular;
}
#version 430 core

#define MAX_POINT_LIGHT_COUNT 4

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec4 a_tangent;
layout(location = 3) in vec2 a_texCoord;

uniform mat4 u_vpMat;
uniform mat4 u_modelMat;
uniform mat3 u_normalMat;
uniform vec3 u_CameraPos;

struct PointLightVertexData
{
	vec3 Position;
};

uniform PointLightVertexData u_PointLightPositions[MAX_POINT_LIGHT_COUNT];
uniform vec3 u_DirectionalLightDir;
uniform bool u_HasDirectionalLight;
uniform int u_PointLightCount;

out VS_OUT
{
	vec2 TexCoords;
	vec3 TS_Position;
	vec3 TS_ViewDir;
	vec3 TS_DirectionalLightDir;
	vec3 TS_PointLightPositions[MAX_POINT_LIGHT_COUNT];
} OUT;

void main()
{
	vec4 p = u_modelMat * vec4(a_position, 1.0);
	OUT.TexCoords = a_texCoord;

    vec3 t = normalize(u_normalMat * a_tangent.xyz);
    vec3 n = normalize(u_normalMat * a_normal);
    t = normalize(t - dot(t, n) * n);
	vec3 b = cross(n, t) * a_tangent.w;

	mat3 tbnMat = transpose(mat3(t, b, n));
	OUT.TS_Position = tbnMat * p.xyz;
	OUT.TS_ViewDir = normalize(tbnMat * (u_CameraPos - p.xyz));
	OUT.TS_DirectionalLightDir = normalize(tbnMat * u_DirectionalLightDir);
	
	for(int i = 0; i < u_PointLightCount; i++)
	{
		OUT.TS_PointLightPositions[i] = tbnMat * u_PointLightPositions[i].Position;
	}

	gl_Position = u_vpMat * p;
}
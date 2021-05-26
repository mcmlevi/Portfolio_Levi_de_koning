#version 430 core

#define MAX_POINT_LIGHT_COUNT 4

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec4 a_tangent;
layout(location = 3) in vec2 a_texCoord;

uniform mat4 u_vpMat;
uniform mat4 u_modelMat;

out VS_OUT
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
} OUT;

void main()
{
	vec4 p = u_modelMat * vec4(a_position, 1.0);

	OUT.Position = p.xyz;
	OUT.Normal = a_normal;
	OUT.TexCoords = a_texCoord;

	gl_Position = u_vpMat * p;
}
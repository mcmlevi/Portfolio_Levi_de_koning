#version 430 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec4 a_tangent;
layout(location = 3) in vec2 a_texCoord;

uniform mat4 u_lightSpaceMatrix;
uniform mat4 u_modelMat;

void main()
{
    gl_Position = u_lightSpaceMatrix * u_modelMat * vec4(a_position, 1.0);
}  
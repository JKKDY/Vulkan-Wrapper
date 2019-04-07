#version 450

layout (binding = 0) uniform UboView
{
	mat4 proj;
	mat4 view;
	mat4 model;
} ubo;


layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTexCoord;

layout (location = 0) out vec2 outTexCoord;


out gl_PerVertex
{
	vec4 gl_Position;
};


void main()
{
	outTexCoord = inTexCoord;
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPos.xyz, 1.0);
}

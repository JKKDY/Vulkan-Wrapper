#version 450

layout (binding = 0) uniform UboView
{
	mat4 viewProj;
} uboView;

layout (binding = 1) uniform UboInstance
{
	mat4 model;
} uboInstance;


layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;

layout (location = 0) out vec3 outColor;


out gl_PerVertex
{
	vec4 gl_Position;
};


void main()
{
	outColor = inColor;
	gl_Position = uboView.viewProj * uboInstance.model * vec4(inPos.xyz, 1.0);
}

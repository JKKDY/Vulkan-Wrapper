#version 450

layout (set = 0, binding = 1) uniform sampler2D samplerColorMap;

layout (location = 0) in vec2 inTexCoord;

layout (location = 0) out vec4 outFragColor;

void main()
{
	outFragColor = texture(samplerColorMap, inTexCoord) * vec4(1.0);
}
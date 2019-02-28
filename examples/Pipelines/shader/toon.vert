#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0)uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 light;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 2) in vec3 inNormal;
layout(location = 1) in vec3 inColor;


layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragViewVec;
layout(location = 3) out vec3 fragLightVec;


out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    vec4 worldPos = ubo.model * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPos;

    fragColor = inColor;

    fragNormal = mat3(ubo.view) * mat3(ubo.model) * inNormal;
    fragViewVec = -(ubo.view * worldPos).xyz;
    fragLightVec = mat3(ubo.view)* (ubo.light - vec3(worldPos));
}
#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragViewVec;
layout(location = 3) in vec3 fragLightVec;


layout(location = 0) out vec4 outColor;


void main(){
     vec3 N = normalize(fragNormal);
    vec3 L = normalize(fragLightVec);
    vec3 V = normalize(fragViewVec);
    vec3 R = reflect(-L, N);

    if (pow(max(dot(R, V), 0.0), 5.0) > 0.6){
        outColor = vec4(vec3(0.7) + fragColor, 1.0);
    } else if (dot (V, N) < 0.5){
        outColor = vec4(fragColor / 10.0, 1.0);
    } else if (max(dot(N, L), 0.0) >= 0.1){
        outColor = vec4(fragColor, 1.0);
    }else {
        outColor = vec4(fragColor / 5.0, 1.0);
    }
}

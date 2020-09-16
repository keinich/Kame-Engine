#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUvCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D tex; 

void main() {
//  outColor = vec4(0.7, 0.9, 0.3, 1.0);
//  outColor = vec4(fragColor, 1);
  outColor = texture(tex, fragUvCoord);
}
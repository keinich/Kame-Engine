#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

void main() {
  outColor = vec4(0.7, 0.9, 0.3, 1.0);
}
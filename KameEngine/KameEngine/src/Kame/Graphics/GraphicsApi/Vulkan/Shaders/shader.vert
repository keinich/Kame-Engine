#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
  vec4 gl_Position;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 uvCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUvCoord;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 fragViewVec;
layout(location = 4) out vec3 fragLightVec;

layout(binding = 0) uniform UBO {
  mat4 model;
  mat4 view;
  mat4 projection;
  vec3 lightPosition;
} ubo;

void main() {
  gl_Position = ubo.projection * ubo.view * ubo.model * vec4(pos, 1.0);
  vec4 worldPos = ubo.model * vec4(pos, 1.0);

  fragColor = color;
  fragUvCoord = uvCoord;
  fragNormal = mat3(ubo.view) * mat3(ubo.model) * inNormal;
  fragViewVec = -(ubo.view * worldPos).xyz;
  fragLightVec = mat3(ubo.view) * (ubo.lightPosition - vec3(worldPos));
}
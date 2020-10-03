#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUvCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragViewVec;
layout(location = 4) in vec3 fragLightVec;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D tex;
layout(binding = 2) uniform sampler2D normalMap;

layout(push_constant) uniform PushConstants {
  bool usePhong;
} pushConts;

void main() {
  //  outColor = vec4(0.7, 0.9, 0.3, 1.0);
  //  outColor = vec4(fragColor, 1);
  //  outColor = texture(tex, fragUvCoord);

  vec3 texColor = texture(tex, fragUvCoord).xyz;
  vec3 N = normalize(texture(normalMap, fragUvCoord).xyz);
//  if (!pushConts.usePhong) {
    texColor = vec3(1.0, 0.9, 0.4);
    N = normalize(fragNormal);
//  }

  vec3 L = normalize(fragLightVec);
  vec3 V = normalize(fragViewVec);
  vec3 R = reflect(-L, N);

  // Phong Shading
  if (pushConts.usePhong) {
    vec3 ambient = texColor * 0.1;
    vec3 diffuse = 1.0 * max(0.0, dot(N, L)) * texColor;
    vec3 specular = pow(max(0.0, dot(R, V)), 16.0) * vec3(1.35);

    outColor = vec4(ambient + diffuse + specular, 1.0);
  } else {
    // Cartoon Shading
    if (pow(max(dot(R, V), 0.0), 5.0) > 0.5) {
      outColor = vec4(1.0, 1.0, 1.0, 1.0);
    } else if (dot(V, N) < 0.5) {
      outColor = vec4(0, 0, 0, 1.0);
    } else if (max(dot(N, L), 0.0) >= 0.1) {
      outColor = vec4(texColor, 1.0);
    } else{
      outColor = vec4(texColor / 5, 1.0);
    }
  }
}
#version 330 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
out vec3 fragNormal;
out vec3 fragWorldPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main(){
  vec4 worldPos = model * vec4(position,1.0);
  fragWorldPos = worldPos.xyz;
  fragNormal = mat3(transpose(inverse(model))) * normal;
  gl_Position = projection * view * worldPos;
}

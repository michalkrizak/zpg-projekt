#version 330 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texCoord;
out vec3 fragNormal;
out vec3 fragWorldPos;
out vec2 fragTexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
void main(){
  vec4 worldPos = model * vec4(position,1.0);
  fragWorldPos = worldPos.xyz;
  fragNormal = normalMatrix * normal;
  fragTexCoord = texCoord;
  gl_Position = projection * view * worldPos;


  //position se ma prenasobit 200 a potom zase nejak vydelit kdyz to chcu dostat zpet a musi se to spravne prevaded. Musi to byt proste udelane tak ze kdyt tam zmenim cislo treba na 200, tak by se to nemelo rozbit. 
}

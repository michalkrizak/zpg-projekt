#version 330 core
in vec3 fragNormal;
in vec3 fragWorldPos;
out vec4 outColor;
uniform vec3 lightPosition;
uniform vec3 lightColor;
void main(){
  vec3 objectColor = vec3(0.385, 0.647, 0.812);
  
  vec3 ambient = 0.1 * lightColor;
  
  // Diffuse
  vec3 norm = normalize(fragNormal);
  vec3 lightDir = normalize(lightPosition - fragWorldPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;
  
  vec3 result = (ambient + diffuse) * objectColor;
  outColor = vec4(result, 1.0);
}

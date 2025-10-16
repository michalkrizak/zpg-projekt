#version 330 core
in vec3 fragColor;
in vec3 fragWorldPos;
out vec4 outColor;
uniform vec3 lightPosition;
uniform vec3 lightColor;
void main(){
  // Simple point light attenuation
  vec3 lightDir = lightPosition - fragWorldPos;
  float dist = length(lightDir);
  float attenuation = 1.0 / (1.0 + 0.05*dist + 0.005*dist*dist);
  vec3 ambient = 0.3 * fragColor;
  vec3 lit = ambient + attenuation * lightColor * fragColor;
  outColor = vec4(lit, 1.0);
}

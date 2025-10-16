#version 330 core
in vec3 vWorldPos;
out vec4 outColor;
uniform vec3 lightPosition;
uniform vec3 lightColor;
float hash(vec2 p){ return fract(sin(dot(p, vec2(127.1,311.7)))*43758.5453); }
void main(){
  // Checker-like tiling with subtle noise
  float tileSize = 2.0;
  vec2 t = floor(vWorldPos.xz / tileSize);
  float checker = mod(t.x + t.y, 2.0);
  vec3 colA = vec3(0.18, 0.35, 0.12);
  vec3 colB = vec3(0.15, 0.32, 0.10);
  float n = hash(t) * 0.08;
  vec3 c = mix(colA, colB, checker) + n;
  // Simple lighting for ground
  vec3 lightDir = lightPosition - vWorldPos;
  float dist = length(lightDir);
  float attenuation = 1.0 / (1.0 + 0.05*dist + 0.005*dist*dist);
  vec3 ambient = 0.4 * c;
  vec3 lit = ambient + attenuation * lightColor * c;
  outColor = vec4(lit,1.0);
}

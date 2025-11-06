#version 330 core
in vec3 vWorldPos;
out vec4 outColor;

#define MAX_LIGHTS 32
#define LIGHT_POINT 0
#define LIGHT_DIRECTIONAL 1
#define LIGHT_SPOT 2
#define LIGHT_AMBIENT 3

struct Light {
    int type;
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    // Spotlight params (cosines are provided)
    float cutOff;
    float outerCutOff;
    // Attenuation
    float constant;
    float linear;
    float quadratic;
    int enabled;
};

uniform Light lights[MAX_LIGHTS];
uniform int numLights;
uniform vec3 viewPos;

// Material coefficients from lighting model
uniform float ra;  // ambient coefficient
uniform float rd;  // diffuse coefficient
uniform float rs;  // specular coefficient
uniform float h;   // shininess

// Texture support
uniform int useTexture;
uniform sampler2D textureSampler;

float hash(vec2 p){ return fract(sin(dot(p, vec2(127.1,311.7)))*43758.5453); }

void main(){
  // Base ground albedo (checker + small noise or texture)
  vec3 baseColor;
  if (useTexture == 1) {
    // Use texture with tiling - hustější opakování pro realistický vzhled trávy
    // UV souřadnice upraveny z rozsahu <0,1> na <0,10> pro více opakování
    vec2 uv = vWorldPos.xz * 10.0; // Scale for denser tiling (10x repeat)
    baseColor = texture(textureSampler, uv).rgb;
  } else {
    // Original procedural checker pattern
    float tileSize = 2.0;
    vec2 t = floor(vWorldPos.xz / tileSize);
    float checker = mod(t.x + t.y, 2.0);
    vec3 colA = vec3(0.18, 0.35, 0.12);
    vec3 colB = vec3(0.15, 0.32, 0.10);
    float n = hash(t) * 0.08;
    baseColor = mix(colA, colB, checker) + n;
  }

  // Assume ground is mostly horizontal
  vec3 normal = vec3(0.0, 1.0, 0.0);
  vec3 viewDir = normalize(viewPos - vWorldPos);

  vec3 result = vec3(0.0);
  for (int i = 0; i < numLights && i < MAX_LIGHTS; ++i) {
    if (lights[i].enabled == 0) continue;
    if (lights[i].type == LIGHT_AMBIENT) {
      result += ra * lights[i].color * lights[i].intensity;
    } else if (lights[i].type == LIGHT_DIRECTIONAL) {
      vec3 L = normalize(-lights[i].direction);
      float diff = max(dot(normal, L), 0.0);
      vec3 diffuse = rd * diff * lights[i].color * lights[i].intensity;
      
      // Specular
      vec3 R = reflect(-L, normal);
      float spec = pow(max(dot(viewDir, R), 0.0), h);
      vec3 specular = rs * spec * lights[i].color * lights[i].intensity;
      
      result += diffuse + specular;
    } else if (lights[i].type == LIGHT_POINT) {
      vec3 toL = lights[i].position - vWorldPos;
      float dist = length(toL);
      vec3 L = dist > 0.0 ? toL / dist : vec3(0.0);
      float attenuation = 1.0 / (lights[i].constant + lights[i].linear * dist + lights[i].quadratic * dist * dist);
      float diff = max(dot(normal, L), 0.0);
      vec3 diffuse = rd * diff * lights[i].color * lights[i].intensity;
      
      // Specular
      vec3 R = reflect(-L, normal);
      float spec = pow(max(dot(viewDir, R), 0.0), h);
      vec3 specular = rs * spec * lights[i].color * lights[i].intensity;
      
      result += (diffuse + specular) * attenuation;
    } else if (lights[i].type == LIGHT_SPOT) {
      vec3 toL = lights[i].position - vWorldPos;
      float dist = length(toL);
      vec3 L = dist > 0.0 ? toL / dist : vec3(0.0);
      float theta = dot(L, normalize(-lights[i].direction));
      float epsilon = lights[i].cutOff - lights[i].outerCutOff;
      float spot = clamp((theta - lights[i].outerCutOff) / max(epsilon, 1e-4), 0.0, 1.0);
      if (spot > 0.0) {
        float attenuation = 1.0 / (lights[i].constant + lights[i].linear * dist + lights[i].quadratic * dist * dist);
        float diff = max(dot(normal, L), 0.0);
        vec3 diffuse = rd * diff * lights[i].color * lights[i].intensity;
        
        // Specular
        vec3 R = reflect(-L, normal);
        float spec = pow(max(dot(viewDir, R), 0.0), h);
        vec3 specular = rs * spec * lights[i].color * lights[i].intensity;
        
        result += (diffuse + specular) * attenuation * spot;
      }
    }
  }

  vec3 lit = result * baseColor;
  outColor = vec4(lit, 1.0);
}

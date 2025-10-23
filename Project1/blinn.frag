#version 330 core
in vec3 fragNormal;
in vec3 fragWorldPos;
out vec4 outColor;

#define MAX_LIGHTS 4

struct Light {
    vec3 position;
    vec3 color;
};

uniform Light lights[MAX_LIGHTS];
uniform int numLights;
uniform vec3 viewPos;

void main() {
    vec3 objectColor = vec3(0.2, 0.2, 0.8);
    vec3 norm = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragWorldPos);
    
    vec3 ambient = vec3(0.1);
    vec3 result = ambient;
    
    for(int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        vec3 lightDir = normalize(lights[i].position - fragWorldPos);
        
        // Diffuse
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lights[i].color;
        
        // Specular (Blinn-Phong)
        vec3 halfDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfDir), 0.0), 32.0);
        vec3 specular = spec * lights[i].color;
        
        result += diffuse + specular;
    }
    
    outColor = vec4(result * objectColor, 1.0);
}

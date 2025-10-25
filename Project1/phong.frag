#version 330 core
in vec3 fragNormal;
in vec3 fragWorldPos;
out vec4 outColor;

#define MAX_LIGHTS 16

struct Light {
    vec3 position;
    vec3 color;
};

uniform Light lights[MAX_LIGHTS];
uniform int numLights;
uniform vec3 viewPos;

void main() {
    vec3 objectColor = vec3(0.2, 0.8, 0.2);
    vec3 norm = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragWorldPos);
    
    vec3 ambient = vec3(0.01);  // Velmi nízký ambient pro noční atmosféru
    vec3 result = ambient;
    
    for(int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        vec3 lightDir = normalize(lights[i].position - fragWorldPos);
        float distance = length(lights[i].position - fragWorldPos);
        
        // Attenuation (útlum): quadratic falloff
        float constant = 1.0;
        float linear = 0.14;
        float quadratic = 0.07;
        float attenuation = 1.0 / (constant + linear * distance + quadratic * distance * distance);
        
        // Diffuse
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lights[i].color;
        
        // Specular (Phong)
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        vec3 specular = spec * lights[i].color;
        
        // Apply attenuation
        diffuse *= attenuation;
        specular *= attenuation;
        
        result += diffuse + specular;
    }
    
    outColor = vec4(result * objectColor, 1.0);
}

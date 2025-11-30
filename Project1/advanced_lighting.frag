#version 330 core
in vec3 fragNormal;
in vec3 fragWorldPos;
in vec2 fragTexCoord;
out vec4 outColor;

#define MAX_LIGHTS 32

// Typy svetel
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
    
    // Pro spotlight
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
uniform vec3 objectColor;


// Material coefficients from lighting model
uniform float ra;  // ambient coefficient
uniform float rd;  // diffuse coefficient
uniform float rs;  // specular coefficient
uniform float h;   // shininess

// Texture support
uniform int useTexture;
uniform sampler2D textureSampler;

vec3 calculatePointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    
    // Attenuation
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                               light.quadratic * distance * distance);

    vec3 Id = light.color * light.intensity * attenuation; 
    vec3 Is = light.color * light.intensity * attenuation;
    
    // Diffuse
    float dot_n_l = max(dot(normal, lightDir), 0.0);
    vec3 I_diffuse = Id * rd * dot_n_l;
    
    // Specular (Phong)
    vec3 reflectDir = reflect(-lightDir, normal);
    float dot_r_c = max(dot(viewDir, reflectDir), 0.0); 
    float specularFactor = pow(dot_r_c, h);

    vec3 I_specular = Is * rs * specularFactor;
    
    return I_diffuse + I_specular;
}

vec3 calculateDirectionalLight(Light light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    vec3 Id = light.color * light.intensity;
    vec3 Is = light.color * light.intensity;
    
    // Diffuse
    float dot_n_l = max(dot(normal, lightDir), 0.0);
    vec3 I_diffuse = Id * rd * dot_n_l;
    
    // Specular (Phong)
    vec3 reflectDir = reflect(-lightDir, normal);
    float dot_r_c = max(dot(viewDir, reflectDir), 0.0);
    float specularFactor = pow(dot_r_c, h);
    
    vec3 I_specular = Is * rs * specularFactor;
    
    return I_diffuse + I_specular;
}

vec3 calculateSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    
    // Spotlight intensity (smooth edges)
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float spotIntensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    if (spotIntensity > 0.0) {
        // Attenuation
        float attenuation = 1.0 / (light.constant + light.linear * distance + 
                                   light.quadratic * distance * distance);
        
        // Diffuse
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = rd * diff * light.color * light.intensity;
        
        // Specular (Phong)
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), h);
        vec3 specular = rs * spec * light.color * light.intensity;
        
        return (diffuse + specular) * attenuation * spotIntensity;
    }
    
    return vec3(0.0);
}

vec3 calculateAmbientLight(Light light) {
    vec3 Ia = light.color * light.intensity;

    return Ia * ra;
}

void main() {
    vec3 norm = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragWorldPos);
    
    // Determine base color (texture or object color)
    vec3 baseColor;
    if (useTexture == 1) {
        // Use actual UV coordinates from the model
        baseColor = texture(textureSampler, fragTexCoord).rgb;
    } else {
        baseColor = objectColor;
    }
    
    vec3 result = vec3(0.0);
    
    for(int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        if (lights[i].enabled == 0) {
            continue;
        }
        
        if (lights[i].type == LIGHT_POINT) {
            result += calculatePointLight(lights[i], norm, fragWorldPos, viewDir);
        }
        else if (lights[i].type == LIGHT_DIRECTIONAL) {
            result += calculateDirectionalLight(lights[i], norm, viewDir);
        }
        else if (lights[i].type == LIGHT_SPOT) {
            result += calculateSpotLight(lights[i], norm, fragWorldPos, viewDir);
        }
        else if (lights[i].type == LIGHT_AMBIENT) {
            result += calculateAmbientLight(lights[i]);
        }
    }
    
    outColor = vec4(result * baseColor, 1.0);
}

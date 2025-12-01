#version 330 core
in vec3 fragNormal;
in vec3 fragWorldPos;
in vec2 fragTexCoord;
out vec4 outColor;

#define MAX_LIGHTS 4

struct Light {
    vec3 position;
    vec3 color;
};

uniform Light lights[MAX_LIGHTS];
uniform int numLights;
uniform vec3 viewPos;
uniform int modelType; // 0=Constant, 1=Lambert, 2=Phong, 3=Blinn
uniform vec3 objectColor;

// Material coefficients
uniform float ra; 
uniform float rd;  
uniform float rs; 
uniform float h;

void main() {
    
    // Constant Shading 
    if (modelType == 0) {
        outColor = vec4(objectColor, 1.0);
        return;
    }
    
    vec3 n = normalize(fragNormal);              
    vec3 c = normalize(viewPos - fragWorldPos);    
    
    // Ambient
    vec3 Ia_global = vec3(0.1);                    
    vec3 ra_vec = objectColor * ra;     
    
    vec3 I = Ia_global * ra_vec; 

    for(int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        
        vec3 l = normalize(lights[i].position - fragWorldPos);
        
        // Attenuation 
        // 1 / (kc + kl*d + kq*d^2)
        float d = length(lights[i].position - fragWorldPos);
        float kc = 1.0; 
        float kl = 0.14; 
        float kq = 0.07;
        float attenuation = 1.0 / (kc + kl * d + kq * d * d);
        
        vec3 Id_source = lights[i].color * attenuation; 
        vec3 Is_source = lights[i].color * attenuation;

        // Lambert / Diffuse
        // Id * rd * max(0, n . l)
        float dot_n_l = max(dot(n, l), 0.0);
        vec3 rd_vec = objectColor * rd; 
        
        vec3 I_diffuse = Id_source * rd_vec * dot_n_l;
        I += I_diffuse;
        
        // Specular Models (Phong & Blinn)
        if (modelType >= 2) {
            vec3 rs_vec = vec3(1.0) * rs;
            float specular_term = 0.0;
            
            if (modelType == 2) {
                // Phong 
                // Is * rs * max(0, r . c)^h
                vec3 r = reflect(-l, n); 
                specular_term = pow(max(dot(r, c), 0.0), h);
                
            } else if (modelType == 3) {
                // Blinn-Phong
                // Is * rs * max(0, n . h_vec)^h
                vec3 h_vec = normalize(l + c);
                specular_term = pow(max(dot(n, h_vec), 0.0), h);
            }
            
            vec3 I_specular = Is_source * rs_vec * specular_term;
            I += I_specular;
        }
    }
    
    outColor = vec4(I, 1.0);
}
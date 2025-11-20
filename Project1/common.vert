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

// === TESTOVANI TRANSFORMACI ===
// Test 1: Zmena w-souradnice
// Nastavte w na ruzne hodnoty (500, 200, atd.) pro overeni spravne implementace
const float w = 1.0; // Standardni hodnota. Zkuste: 500.0, 200.0, 20.0

// Test 2: Vlastni transformacni matice
// Jednotkova matice s upravenym prvkem [3][3] (ctvrty radek, ctvrty sloupec)
mat4 customTransform = mat4(
    1.0, 0.0, 0.0, 0.0,  // prvni sloupec
    0.0, 1.0, 0.0, 0.0,  // druhy sloupec
    0.0, 0.0, 1.0, 0.0,  // treti sloupec
    0.0, 0.0, 0.0, 1.0   // ctvrty sloupec - zkuste: 20.0 misto 1.0
);

void main(){
    // Aplikujeme model transformaci s upravenou w-souradnici
    vec4 worldPos = model * vec4(position, w);
    
    // KLICOVE: Normalizace pomoci w pro spravne vypocty osvetleni
    // Kdyz zmenime w, musime vydelit xyz slozky hodnotou w,
    // abychom dostali skutecnou pozici v prostoru
    fragWorldPos = worldPos.xyz / worldPos.w;
    
    // Normaly nejsou ovlivneny w-souradnici
    fragNormal = normalMatrix * normal;
    fragTexCoord = texCoord;
    
    // Aplikace vlastni transformace a projekce
    // Vlastni transformace se aplikuje PRED view a projection
    gl_Position = projection * view * customTransform * worldPos;
}

/*
=== VYSVETLENI A TEORIE ===

1. HOMOGENNI SOURADNICE (w-komponenta):
   - Homogenni souradnice maji tvar (x, y, z, w)
   - Pro prevod do kartezskych souradnic: (x/w, y/w, z/w)
   - Standardne w = 1.0 (zadne zmeny)
   - Pri w != 1.0 se pozice "prepocita" - vetsi w = menseni objektu
   
2. PERSPEKTIVNI DELENI:
   - GPU automaticky deli gl_Position.xyz hodnotou gl_Position.w
   - Proto musime take vydelit fragWorldPos.xyz hodnotou worldPos.w
   - Tim zajistime konzistenci mezi world pozici a clip space pozici
   
3. VLASTNI TRANSFORMACE:
   - Prvek [3][3] matice (ctvrty radek, ctvrty sloupec) ovlivnuje w-komponentu
   - Pri hodnote 20.0: w_new = w_old * 20.0
   - Vysledek: objekt se "zmensi" 20x (protoze xyz se deli vetsi hodnotou w)
   
4. JAK OVERIT SPRAVNOST:
   - Pri w = 500: objekty by mely byt 500x mensi, ale s korektnim osvetlenim
   - Pri customTransform[3][3] = 20: objekty 20x mensi
   - Osvetleni musi zustat spravne (normaly a pozice konzistentni)
   - Objekty by nemely "zmizet" ani se rozbit
   
5. PROC TO FUNGUJE:
   - Deleni worldPos.xyz / worldPos.w zajisti spravnou world pozici pro osvetleni
   - GPU provede automaticke perspektivni deleni na gl_Position
   - Zachovava se matematicka konzistence mezi ruznyma prostory
*/

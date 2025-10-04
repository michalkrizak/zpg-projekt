#include "house.h"



const float house[288] = {

    // Pøední strana
    -0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // dolní levý
     0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // dolní pravý
     0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // horní pravý

    -0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // dolní levý
     0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // horní pravý
    -0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // horní levý

    // Zadní strana
    -0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // dolní levý
     0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // dolní pravý
     0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // horní pravý

    -0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // dolní levý
     0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // horní pravý
    -0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // horní levý

    // Levá strana
    -0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // dolní levý
    -0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // dolní pravý
    -0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // horní pravý

    -0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // dolní levý
    -0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // horní pravý
    -0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // horní levý

    // Pravá strana
     0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // dolní levý
     0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // dolní pravý
     0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // horní pravý

     0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // dolní levý
     0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // horní pravý
     0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // horní levý

     // Horní strana
     -0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // levý pøední
      0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // pravý pøední
      0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // pravý zadní

     -0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // levý pøední
      0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // pravý zadní
     -0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // levý zadní

     // Spodní strana
     -0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // levý pøední
      0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // pravý pøední
      0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // pravý zadní

     -0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // levý pøední
      0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // pravý zadní
     -0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // levý zadní

     // Støecha (trojúhelníky)

     // Pøední trojúhelník
     -0.5f,  0.5f,  0.5f,  0.7f, 0.2f, 0.0f, // levý vrchol
      0.5f,  0.5f,  0.5f,  0.7f, 0.2f, 0.0f, // pravý vrchol
      0.0f,  1.0f,  0.0f,  0.7f, 0.2f, 0.0f, // vrchol støechy

      // Zadní trojúhelník
      -0.5f,  0.5f, -0.5f,  0.7f, 0.2f, 0.0f, // levý vrchol
       0.5f,  0.5f, -0.5f,  0.7f, 0.2f, 0.0f, // pravý vrchol
       0.0f,  1.0f,  0.0f,  0.7f, 0.2f, 0.0f, // vrchol støechy

       // Levý trojúhelník
       -0.5f,  0.5f,  0.5f,  0.7f, 0.2f, 0.0f, // levý vrchol
       -0.5f,  0.5f, -0.5f,  0.7f, 0.2f, 0.0f, // pravý vrchol
        0.0f,  1.0f,  0.0f,  0.7f, 0.2f, 0.0f, // vrchol støechy

        // Pravý trojúhelník
         0.5f,  0.5f,  0.5f,  0.7f, 0.2f, 0.0f, // levý vrchol
         0.5f,  0.5f, -0.5f,  0.7f, 0.2f, 0.0f, // pravý vrchol
         0.0f,  1.0f,  0.0f,  0.7f, 0.2f, 0.0f, // vrchol støechy
};


const int houseVertexCount = sizeof(house) / (6 * sizeof(float));
const int houseDataSize = sizeof(house);
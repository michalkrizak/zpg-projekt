#include "house.h"



const float house[288] = {

    // P�edn� strana
    -0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // doln� lev�
     0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // doln� prav�
     0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // horn� prav�

    -0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // doln� lev�
     0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // horn� prav�
    -0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // horn� lev�

    // Zadn� strana
    -0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // doln� lev�
     0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // doln� prav�
     0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // horn� prav�

    -0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // doln� lev�
     0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // horn� prav�
    -0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // horn� lev�

    // Lev� strana
    -0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // doln� lev�
    -0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // doln� prav�
    -0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // horn� prav�

    -0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // doln� lev�
    -0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // horn� prav�
    -0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // horn� lev�

    // Prav� strana
     0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // doln� lev�
     0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // doln� prav�
     0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // horn� prav�

     0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // doln� lev�
     0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // horn� prav�
     0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // horn� lev�

     // Horn� strana
     -0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // lev� p�edn�
      0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // prav� p�edn�
      0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // prav� zadn�

     -0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // lev� p�edn�
      0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // prav� zadn�
     -0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // lev� zadn�

     // Spodn� strana
     -0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // lev� p�edn�
      0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // prav� p�edn�
      0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // prav� zadn�

     -0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f, // lev� p�edn�
      0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // prav� zadn�
     -0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f, // lev� zadn�

     // St�echa (troj�heln�ky)

     // P�edn� troj�heln�k
     -0.5f,  0.5f,  0.5f,  0.7f, 0.2f, 0.0f, // lev� vrchol
      0.5f,  0.5f,  0.5f,  0.7f, 0.2f, 0.0f, // prav� vrchol
      0.0f,  1.0f,  0.0f,  0.7f, 0.2f, 0.0f, // vrchol st�echy

      // Zadn� troj�heln�k
      -0.5f,  0.5f, -0.5f,  0.7f, 0.2f, 0.0f, // lev� vrchol
       0.5f,  0.5f, -0.5f,  0.7f, 0.2f, 0.0f, // prav� vrchol
       0.0f,  1.0f,  0.0f,  0.7f, 0.2f, 0.0f, // vrchol st�echy

       // Lev� troj�heln�k
       -0.5f,  0.5f,  0.5f,  0.7f, 0.2f, 0.0f, // lev� vrchol
       -0.5f,  0.5f, -0.5f,  0.7f, 0.2f, 0.0f, // prav� vrchol
        0.0f,  1.0f,  0.0f,  0.7f, 0.2f, 0.0f, // vrchol st�echy

        // Prav� troj�heln�k
         0.5f,  0.5f,  0.5f,  0.7f, 0.2f, 0.0f, // lev� vrchol
         0.5f,  0.5f, -0.5f,  0.7f, 0.2f, 0.0f, // prav� vrchol
         0.0f,  1.0f,  0.0f,  0.7f, 0.2f, 0.0f, // vrchol st�echy
};


const int houseVertexCount = sizeof(house) / (6 * sizeof(float));
const int houseDataSize = sizeof(house);
#include "plain.h"

/**
 * @file plain.h
 *
 * @brief Plain model
 *
 * @details  Command for rendering
 *           glDrawArrays(GL_TRIANGLES, 0, 6);
 *
 * @author Martin Nemec
 *
 * @year 2023
 **/


const float plain[36] = {
    1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
   -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,

   -1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f,
   -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f

};

const int plainVertexCount = sizeof(plain) / (6 * sizeof(float));
const int plainDataSize = sizeof(plain);
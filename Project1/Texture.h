#pragma once
#include <string>
#include <GL/glew.h>

class Texture {
private:
    GLuint textureID;
    int width, height, channels;

public:
    Texture(const std::string& filepath);
    ~Texture();

    void bind(GLuint textureUnit = 0) const;
    void unbind() const;
    GLuint getID() const { return textureID; }
};

#pragma once

#include <memory>
#include <string>

#include <glad/glad.h>

class Texture {
public:
    Texture(GLuint texture);
    ~Texture();

    static std::shared_ptr<Texture> create(const std::string& file_path);

    void bind(GLenum slot) const;
    static void unbind();
private:
    GLuint texture;
};

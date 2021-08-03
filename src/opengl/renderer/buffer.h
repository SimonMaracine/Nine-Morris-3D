#pragma once

#include <memory>
#include <cstddef>

#include <glad/glad.h>

class Buffer {
public:
    enum class Type {
        Array, Index, Uniform
    };

    Buffer(GLuint buffer, Type type);
    ~Buffer();

    static std::shared_ptr<Buffer> create(std::size_t size);
    static std::shared_ptr<Buffer> create(const void* data,
                                          std::size_t size);
    static std::shared_ptr<Buffer> create_index(const unsigned int* data,
                                                std::size_t size);
    static std::shared_ptr<Buffer> create_uniform(const void* data,
                                                  std::size_t size);

    void bind() const;
    // This only unbinds the ARRAY_BUFFER, not ELEMENT_ARRAY_BUFFER or UNIFORM_BUFFER
    static void unbind();

    void update_data(const void* data, std::size_t size) const;
private:
    GLuint buffer;
    Type type;

    friend class VertexArray;
    friend class Shader;
};

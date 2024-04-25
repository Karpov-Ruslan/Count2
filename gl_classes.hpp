#pragma once

#include "auxiliary.hpp"

namespace OpenGL {

class Object {
public:
    explicit Object() noexcept = default;

    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;
    Object(Object&&) = delete;
    Object& operator=(Object&&) = delete;

    operator GLuint() noexcept {
        return data;
    }

    operator GLuint*() noexcept {
        return &data;
    }

    virtual ~Object() noexcept = 0;
protected:
    GLuint data = 0u;
};

Object::~Object() noexcept = default;

class ObjectArray {
public:
    explicit ObjectArray(const GLsizei size) noexcept : data(size) {
        if (!size) {
            std::cerr << "VBO Constructor: VBO size is zero" << std::endl;
            std::terminate();
        }
    };

    ObjectArray(const ObjectArray&) = delete;
    ObjectArray& operator=(const ObjectArray&) = delete;
    ObjectArray(ObjectArray&&) = delete;
    ObjectArray& operator=(ObjectArray&&) = delete;

    operator GLuint() noexcept {
        return data[0];
    }

    GLuint& operator[](const size_t num) {
        return data[num];
    }

    operator GLuint*() noexcept {
        return data.data();
    }

    virtual ~ObjectArray() noexcept = 0;
protected:
    std::vector<GLuint> data;
};

ObjectArray::~ObjectArray() noexcept = default;



class GraphicsProgram final: public Object {
public:
    explicit GraphicsProgram(const std::string &vertex, const std::string &fragment, const std::string &tessellation_cont = "", const std::string &tessellation_eval = "", const std::string &geometry = "") noexcept {
        data = programLoader(vertex, fragment, tessellation_cont, tessellation_eval, geometry);
    }

    ~GraphicsProgram() noexcept final {
        GL_CALL(glDeleteProgram(data))
    }
};

class ComputeProgram final : public Object {
public:
    explicit ComputeProgram(const std::string& filename) noexcept {
        data = computeProgramLoader(filename);
    }

    ~ComputeProgram() noexcept final {
        GL_CALL(glDeleteProgram(data))
    }
};

class VertexBufferObject final : public ObjectArray {
public:
    explicit VertexBufferObject(const GLsizei size = 1) noexcept : ObjectArray(size) {
        GL_CALL(glGenBuffers(size, data.data()))
    }

    ~VertexBufferObject() noexcept override {
        GL_CALL(glDeleteBuffers(data.size(), data.data()))
    }
};

using VBO = VertexBufferObject;



class VertexArrayObject final : public ObjectArray {
public:
    explicit VertexArrayObject(const GLsizei size = 1) noexcept : ObjectArray(size) {
        GL_CALL(glGenVertexArrays(size, data.data()))
    }

    ~VertexArrayObject() noexcept override {
        GL_CALL(glDeleteVertexArrays(data.size(), data.data()))
    }
};

using VAO = VertexArrayObject;



class Texture final : public ObjectArray {
public:
    explicit Texture(const GLsizei size = 1) noexcept : ObjectArray(size) {
        GL_CALL(glGenTextures(size, data.data()))
    }

    ~Texture() noexcept override {
        GL_CALL(glDeleteTextures(data.size(), data.data()))
    }
};

class Framebuffer final : public ObjectArray {
public:
    explicit Framebuffer(const GLsizei size = 1) noexcept : ObjectArray(size) {
        GL_CALL(glGenFramebuffers(size, data.data()))
    }

    ~Framebuffer() noexcept override {
        GL_CALL(glDeleteFramebuffers(data.size(), data.data()))
    }
};

class Sampler final : public ObjectArray {
public:
    explicit Sampler(const GLsizei size = 1) noexcept : ObjectArray(size) {
        GL_CALL(glGenSamplers(size, data.data()))
    }

    ~Sampler() noexcept override {
        GL_CALL(glDeleteSamplers(data.size(), data.data()))
    }
};

class RenderBuffer final : public ObjectArray {
public:
    explicit RenderBuffer(const GLsizei size = 1) noexcept : ObjectArray(size) {
        GL_CALL(glGenRenderbuffers(size, data.data()))
    }

    ~RenderBuffer() noexcept override {
        GL_CALL(glDeleteRenderbuffers(data.size(), data.data()))
    }
};

class Query final : public ObjectArray {
public:
    explicit Query(const GLsizei size = 1) noexcept : ObjectArray(size) {
        GL_CALL(glGenQueries(size, data.data()))
    }

    ~Query() noexcept override {
        GL_CALL(glDeleteQueries(data.size(), data.data()))
    }
};

class TransformFeedback final: public ObjectArray {
public:
    explicit TransformFeedback(const GLsizei size = 1) noexcept : ObjectArray(size) {
        GL_CALL(glGenTransformFeedbacks(size, data.data()))
    }

    ~TransformFeedback() noexcept override {
        GL_CALL(glDeleteTransformFeedbacks(data.size(), data.data()))
    }
};

}
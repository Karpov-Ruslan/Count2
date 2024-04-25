#pragma once

#include <format>
#include <cmath>
#include <fstream>
#include <iostream>
#include <cstring>
#include <GL/glew.h>
#include <GL/gl.h>
#include <SFML/Graphics.hpp>

#define GL_CALL(func) func;\
                      \
{\
    GLenum err = glGetError();\
    if (err != GL_NO_ERROR) {\
        std::cerr << "OpenGL Error: "<< __FILE__ << ":" << __LINE__ << "\n";\
        switch (err) {\
            case GL_INVALID_ENUM: {\
                std::cerr << "GL_INVALID_ENUM" << std::endl;\
                break;\
            }\
            case GL_INVALID_VALUE: {\
                std::cerr << "GL_INVALID_VALUE" << std::endl;\
                break;\
            }\
            case GL_INVALID_OPERATION: {\
                std::cerr << "GL_INVALID_OPERATION" << std::endl;\
                break;\
            }\
            case GL_STACK_OVERFLOW: {\
                std::cerr << "GL_STACK_OVERFLOW" << std::endl;\
                break;\
            }\
            case GL_STACK_UNDERFLOW: {\
                std::cerr << "GL_STACK_UNDERFLOW" << std::endl;\
                break;\
            }\
            case GL_OUT_OF_MEMORY: {\
                std::cerr << "GL_OUT_OF_MEMORY" << std::endl;\
                break;\
            }\
        }             \
        std::terminate();                  \
    }\
}

const auto getMousePosXY = [](const sf::RenderWindow &window){
    sf::Vector2f mouse_pos = sf::Vector2f(sf::Mouse::getPosition(window))/static_cast<float>(window.getSize().x);
    mouse_pos.y = 1.0f - mouse_pos.y;
    mouse_pos = 2.0f*mouse_pos - sf::Vector2f(1.0f, 1.0f);
    return mouse_pos;
};

const auto newMat = [](GLfloat* mvp, const GLfloat angleOX, const GLfloat angleOY){
    const GLfloat arr[] = {std::cos(angleOY),   std::sin(angleOX)*std::sin(angleOY),    -std::cos(angleOX)*std::sin(angleOY),
                           0.0f,                std::cos(angleOX),                      std::sin(angleOX),
                           std::sin(angleOY),  -std::cos(angleOY)*std::sin(angleOX),    std::cos(angleOY)*std::cos(angleOX)};
    std::memcpy(mvp, arr, sizeof(arr));
};

namespace {

const auto shaderLoader = [](const std::string &shaderName, std::string &shaderCode) -> bool {
    std::ifstream fopen;
    fopen.open(shaderName);
    if (fopen.is_open()) {
        std::getline(fopen, shaderCode, '\0');
//            std::cout << shaderCode << std::endl; // Debug
        fopen.close();
        return true;
    } else {
        std::cerr << "File is not opened: " + shaderName << std::endl;
        return false;
    }
};

const auto shaderCompile = [](GLuint program, GLuint& shaderNum, const std::string &shaderName, GLenum shaderType) {
    shaderNum = GL_CALL(glCreateShader(shaderType))
    std::string shaderCode;
    if (!shaderLoader(shaderName, shaderCode)) {
        return false;
    }

    auto ptr = shaderCode.data();
    GLint size = shaderCode.size();
    GL_CALL(glShaderSource(shaderNum, 1, &ptr, &size))
    GL_CALL(glCompileShader(shaderNum))
    GLint compileStatus = 0u;
    GL_CALL(glGetShaderiv(shaderNum, GL_COMPILE_STATUS, &compileStatus))
    if (compileStatus == GL_FALSE) {
        std::cerr << "Shader is not compiled: " + shaderName << std::endl;
        GLint infoLen = 0;
        GL_CALL(glGetShaderiv(shaderNum, GL_INFO_LOG_LENGTH, &infoLen))
        if (infoLen > 1) {
            std::string infoLog;
            infoLog.resize(infoLen);
            GL_CALL(glGetShaderInfoLog(shaderNum, infoLen, nullptr, infoLog.data()))
            std::cerr << "Error compilation:\n" << infoLog << std::endl;
        }
        return false;
    }

    GL_CALL(glAttachShader(program, shaderNum))

    return true;
};

const auto programLink = [](GLuint& program){
    GL_CALL(glLinkProgram(program))
    GLint programStatus = 0;
    GL_CALL(glGetProgramiv(program, GL_LINK_STATUS, &programStatus))

    if (programStatus == GL_FALSE) {
        std::cerr << "Program is not linked(" << std::endl;
        GLint infoLen = 0;
        GL_CALL(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen))
        if (infoLen > 1) {
            std::string infoLog;
            infoLog.resize(infoLen);
            GL_CALL(glGetProgramInfoLog(program, infoLen, nullptr, infoLog.data()))
            std::cerr << "Error linking:\n" << infoLog << std::endl;
        }
    }

    return (programStatus == GL_TRUE);
};

}

GLuint programLoader(const std::string &vertex, const std::string &fragment, const std::string &tessellation_cont = "", const std::string &tessellation_eval = "", const std::string &geometry = "") {
    bool STATUS = true;
    GLuint program = 0u;
    program = GL_CALL(glCreateProgram())

    GLuint vertexShader = 0u;
    GLuint fragmentShader = 0u;
    GLuint tessellationControlShader = 0u;
    GLuint tessellationEvaluationShader = 0u;
    GLuint geometryShader = 0u;

    STATUS &= (shaderCompile(program, vertexShader, vertex, GL_VERTEX_SHADER) && shaderCompile(program, fragmentShader, fragment, GL_FRAGMENT_SHADER));

    if (!tessellation_cont.empty() && !tessellation_eval.empty()) {
        STATUS &= (shaderCompile(program, tessellationControlShader, tessellation_cont, GL_TESS_CONTROL_SHADER) && shaderCompile(program, tessellationEvaluationShader, tessellation_eval, GL_TESS_EVALUATION_SHADER));
    }
    if (!geometry.empty()) {
        STATUS &= shaderCompile(program, geometryShader, geometry, GL_GEOMETRY_SHADER);
    }

    STATUS &= programLink(program);

    GL_CALL(glDetachShader(program, vertexShader))
    GL_CALL(glDetachShader(program, fragmentShader))
    if (!tessellation_cont.empty() && !tessellation_eval.empty()) {
        GL_CALL(glDetachShader(program, tessellationControlShader))
        GL_CALL(glDetachShader(program, tessellationEvaluationShader))
    }
    if (!geometry.empty()) {
        GL_CALL(glDetachShader(program, geometryShader))
    }

    GL_CALL(glDeleteShader(vertexShader))
    GL_CALL(glDeleteShader(fragmentShader))
    GL_CALL(glDeleteShader(tessellationControlShader))
    GL_CALL(glDeleteShader(tessellationEvaluationShader))
    GL_CALL(glDeleteShader(geometryShader))

    if (!STATUS) {
        GL_CALL(glDeleteProgram(program))
        std::terminate(); // TODO: Change It
        return 0u;
    }

    return program;
}

GLuint computeProgramLoader(const std::string& filename) {
    bool STATUS = true;
    GLuint program = 0u;
    program = GL_CALL(glCreateProgram())
    GLuint computeShader = 0u;

    STATUS &= shaderCompile(program, computeShader, filename, GL_COMPUTE_SHADER);

    STATUS &= programLink(program);

    GL_CALL(glDetachShader(program, computeShader))
    GL_CALL(glDeleteShader(computeShader))

    if (!STATUS) {
        GL_CALL(glDeleteProgram(program))
        std::terminate(); // TODO: Change It
        return 0u;
    }

    return program;
}
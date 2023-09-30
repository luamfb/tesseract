/*
 * Copyright (C) 2023 Luana C. M. de F. Barbosa
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "glad/glad.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "shader.h"

namespace {

std::string readFile(const char *path) {
    std::ifstream inFile{path};
    if (!inFile) {
        std::cout << "error: failed to read file '" << path
            << "'" << std::endl;
        exit(-1);
    }

    std::stringstream strStream;
    strStream << inFile.rdbuf();
    return strStream.str();
}

unsigned int compileShader(const char *source, GLenum type) {
    unsigned int shaderId = glCreateShader(type);
    glShaderSource(shaderId, 1, &source, NULL);
    glCompileShader(shaderId);

    int success;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "error: shader compilation failed:\n"
            << infoLog << std::endl;
        exit(-1);
    }

    return shaderId;
}

unsigned int linkShaderProgram(
        unsigned int vertexShader,
        unsigned int fragmentShader) {

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
        std::cout << "error: shader program linking error:\n"
            << infoLog << std::endl;
        exit(-1);
    }

    return shaderProgram;
}

} // anonymous

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
    unsigned int vertexShader;
    unsigned int fragmentShader;

    {
        std::string vertexCode{readFile(vertexPath)};
        std::string fragmentCode{readFile(fragmentPath)};

        vertexShader = compileShader(vertexCode.c_str(),
            GL_VERTEX_SHADER);
        fragmentShader = compileShader(fragmentCode.c_str(),
            GL_FRAGMENT_SHADER);
    } // free vertexCode and fragmentCode

    id_ = linkShaderProgram(vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::setInt(const char *name, int value) const {
    glUniform1i(glGetUniformLocation(id_, name), value);
}

void Shader::setBool(const char *name, bool value) const {
    glUniform1i(glGetUniformLocation(id_, name), (int) value);
}

void Shader::setFloat(const char *name, float value) const {
    glUniform1f(glGetUniformLocation(id_, name), value);
}

void Shader::setMat4(const char *name, const float *value) const {
    glUniformMatrix4fv(glGetUniformLocation(id_, name),
        1,
        GL_FALSE,
        value);
}

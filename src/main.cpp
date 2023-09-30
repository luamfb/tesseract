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

#include <iostream>
#include <cstdlib>
#include <cmath>

#include "glad/glad.h" // must be included before GLFW
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shader.h"

#define INITIAL_WIDTH   800
#define INITIAL_HEIGHT  600

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

GLFWwindow *initAll() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(INITIAL_WIDTH, INITIAL_HEIGHT,
        "Tesseract", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    // note: somehow fails if we haven't already created the GLFW window.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    glViewport(0, 0, INITIAL_WIDTH, INITIAL_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}


// The usual projection matrix computed by GLM relies on w=1,
// in particular when computing the z coordinate, which looks like
//      z <- z * k1 + k0
// where k1 and k0 depend on zNear and zFar.
//
// The constant factor k0 is given as the w coordinate coefficient,
// assuming w=1, which we can no longer do here. Instead,
// we compute this coefficient here and pass it back to our caller,
// so it can set it as a uniform to be passed on to the vertex shader,
// which will directly add that to the z coordinate.
//
glm::mat4 projection_matrix(
        float fovy,
        float xy_aspect,
        float zNear,
        float zFar,
        float &constFactor) {

    const float tanHalfFovy = tan(fovy / 2.0f);

    glm::mat4 mat(0.0f);
    mat[0][0] = 1.0f / (xy_aspect * tanHalfFovy);
    mat[1][1] = 1.0f / tanHalfFovy;

    // column-major order: the row is in the second index.
    // (Also, counting starts from zero.)
    mat[2][3] = -1.0f;

    mat[2][2] = - (zFar + zNear) / (zFar - zNear);

    // would usually be the coefficient to multiply w by
    constFactor = - (2.0f * zFar * zNear) / (zFar - zNear);

    // here's the big difference from the ordinary projection matrix,
    // where we take the 4th coordinate into account.
    mat[3][0] = 1.0f;
    mat[3][1] = 1.0f;
    mat[3][2] = 1.0f;
    mat[3][3] = 1.0f;

    return mat;
}

void setupVertexObjects(
        unsigned int &vao,
        unsigned int &vbo,
        unsigned int &ebo) {

    float vertices[] = {
        // position (xyzw)              color (RGB)
        -0.5f, -0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, -0.5f,     1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -0.5f,     1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,

         0.5f, -0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, -0.5f,     1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, -0.5f,     1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f
    };

    unsigned int indices[] = {
        0,  1,
        0,  2,
        0,  4,
        0,  8,
        1,  3,
        1,  5,
        1,  9,
        2,  3,
        2,  6,
        2,  10,
        3,  7,
        3,  11,
        4,  5,
        4,  6,
        4,  12,
        5,  7,
        5,  13,
        6,  7,
        6,  14,
        7,  15,
        8,  9,
        8,  10,
        8,  12,
        9,  11,
        9,  13,
        10, 11,
        10, 14,
        11, 15,
        12, 13,
        12, 14,
        13, 15,
        14, 15,
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        indices,
        GL_STATIC_DRAW);

    size_t stride = 7 * sizeof(float);
    // position
    glVertexAttribPointer(
        0,
        4,
        GL_FLOAT,
        GL_FALSE,
        stride,
        (void*) 0
    );
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        stride,
        (void*) (4 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void renderLoop(
        GLFWwindow *window,
        Shader &shaderProg,
        unsigned int vao) {

    shaderProg.use();

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(vao);

        glm::mat4 model = glm::mat4(1.0f);
        /*
        model = glm::rotate(model,
            (float)glfwGetTime() * glm::radians(50.0f),
            glm::vec3(0.5f, 1.0f, 0.0f));
            */

        // Instead of constructing a view matrix with glm::translate,
        // which relies on w=1, we hardcode camera position at the
        // vertex shader (could be passed as a uniform too).

        float zConstOffset;
        glm::mat4 projection = projection_matrix(
            glm::radians(45.0f),    // field of view (FoV) in y axis
            800.0f / 600.0f,        // width / height aspect ratio
            0.1f,                   // near plane z position
            100.0f,                 // far plane z position
            zConstOffset
        );

        shaderProg.setMat4("model", glm::value_ptr(model));
        shaderProg.setMat4("projection", glm::value_ptr(projection));
        shaderProg.setFloat("zConstOffset", zConstOffset);

        // 32 edges in cube * 2 points per edge
        glDrawElements(GL_LINES, 64, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glBindVertexArray(0); // not sure if we have to
}

int main(int argc, char **argv) {
    GLFWwindow *window = initAll();

    unsigned int vao, vbo, ebo;
    setupVertexObjects(vao, vbo, ebo);

    Shader shaderProg{"shaders/vertex.txt", "shaders/frag.txt"};

    renderLoop(window, shaderProg, vao);

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    glfwTerminate();
    return 0;
}

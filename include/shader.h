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

#ifndef SHADER_H
#define SHADER_H

class Shader {
public:
    unsigned int getID() { return id_; }

    // reads files and builds shader program
    Shader(const char *vertexPath, const char *fragmentPath);

    // use / activate shader program
    void use() { glUseProgram(id_); }

    // utility uniform functions
    void setInt(const char *name, int value) const;
    void setBool(const char *name, bool value) const;
    void setFloat(const char *name, float value) const;
    // assumes a single, normalized matrix
    void setMat4(const char *name, const float *value) const;
private:
    unsigned int id_; // the program ID
};

#endif /* SHADER_H */

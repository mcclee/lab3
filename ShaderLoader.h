
#ifndef ShaderLoader_h
#define ShaderLoader_h

#include <glad/glad.h> // get openGL pointers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm.hpp>

class Shader
{
public:
    unsigned int ID;
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    void use();
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
};

#endif /* ShaderLoader_h */

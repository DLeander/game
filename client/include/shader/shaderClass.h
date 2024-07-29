#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/gl.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

std::string get_file_contents(const char *filename);

class Shader
{
    public:
        // Variables
        GLuint ID;

        // Make this into a map where all locations are stored:
        // std::map<std::string, GLuint> locations;
        GLuint transformLoc;

        // Methods
        Shader();
        Shader(const char *vertexPath, const char *fragmentPath);
        void Activate();
        void Delete();
    private:
        void compileErrors(unsigned int shader, const char* type);
};

#endif // SHADER_CLASS_H
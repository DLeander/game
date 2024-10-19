#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/gl.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

std::string get_file_contents(const char *filename);

class CSHADER
{
    public:
        // Variables
        GLuint m_ID;

        // Make this into a map where all locations are stored:
        // std::map<std::string, GLuint> locations;
        GLuint m_modelLoc;
        GLuint m_viewLoc;
        GLuint m_projectionLoc;
        GLuint m_textureLoc;


        // Methods
        CSHADER();
        CSHADER(const char *vertexPath, const char *fragmentPath);
        void Activate();
        void Delete();
    private:
        void compileErrors(unsigned int shader, const char* type);
};

#endif // SHADER_CLASS_H
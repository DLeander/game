#include "shaderClass.h"

std::string get_file_contents(const char *filename){
    std::ifstream in(filename, std::ios::binary);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    throw std::runtime_error(std::string("Failed to open file: ") + filename + std::string("."));
}

CSHADER::CSHADER(){
    m_ID = 0;
}

CSHADER::CSHADER(const char* vertexFile, const char* fragmentFile){
    std::string vertexCode = get_file_contents(vertexFile);
    std::string fragmentCode = get_file_contents(fragmentFile);

    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    // Create shaders.
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	m_ID = glCreateProgram();

	glAttachShader(m_ID, vertexShader);
	glAttachShader(m_ID, fragmentShader);

	glLinkProgram(m_ID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void CSHADER::Activate(){
    glUseProgram(m_ID);
}

void CSHADER::DeActivate(){
    glUseProgram(0);
}

void CSHADER::Delete(){
    glDeleteProgram(m_ID);
}

void CSHADER::compileErrors(unsigned int uiShader, const char* type){
    GLint hasCompiled;
    char infoLog[1024];
    std::string shaderType = type;
    if (!shaderType.compare("PROGRAM")){
        glGetShaderiv(uiShader, GL_COMPILE_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE){
            glGetShaderInfoLog(uiShader, 1024, NULL, infoLog);
            std::cout << "SHADER_COMPILATION_ERROR for: " << type << "\n" << "\n";
        }
    } else {
        glGetProgramiv(uiShader, GL_LINK_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE){
            glGetProgramInfoLog(uiShader, 1024, NULL, infoLog);
            std::cout << "PROGRAM_LINKING_ERROR for: " << type << "\n" << "\n";
        }
    }
}
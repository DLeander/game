#include "texture.h"

Texture::Texture(const char* texturePath, GLenum type, GLenum slot, GLenum format, GLenum pixelType){
    // Texture Variables
    m_Type = type;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* ucTextureData;
    ucTextureData = stbi_load(texturePath, &m_iTextureWidth, &m_iTextureHeight, &m_iChannels, 0);
    if (!ucTextureData) {
        std::cerr << "Failed to load texture at " << texturePath << "\n";
    }
    
    glGenTextures(1, &m_ID);
    glActiveTexture(slot);
    glBindTexture(type, m_ID);

    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(type, 0, GL_RGBA, m_iTextureWidth, m_iTextureHeight, 0, format, pixelType, ucTextureData);
    glGenerateMipmap(type);

    stbi_image_free(ucTextureData);
    glBindTexture(type, 0);
}

Texture::Texture(unsigned char* texture, GLenum type, GLenum slot, GLenum format, GLenum pixelType, int iWidth, int iHeight){
    // Texture Variables
    m_Type = type;
    m_iTextureWidth=iWidth;
    m_iTextureHeight=iHeight;
    m_iChannels=3;
    stbi_set_flip_vertically_on_load(true);

    if (!texture) {
        std::cerr << "Failed to load texture\n";
    }
    
    glGenTextures(1, &m_ID);
    glActiveTexture(slot);
    glBindTexture(type, m_ID);

    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(type, 0, GL_RGBA, m_iTextureWidth, m_iTextureHeight, 0, format, pixelType, texture);
    glGenerateMipmap(type);

    glBindTexture(type, 0);
}

void Texture::textureUnit(CSHADER* shader, const char* uniform, GLuint unit){
    shader->Activate();
    glUniform1i(glGetUniformLocation(shader->m_ID, uniform), unit);
}

void Texture::Bind(){
    glBindTexture(m_Type, m_ID);
}

void Texture::Unbind(){
    glBindTexture(m_Type, 0);
}

void Texture::Delete(){
    if (m_ID != 0){
        glDeleteTextures(1, &m_ID);
    }
}
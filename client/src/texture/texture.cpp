#include "texture.h"

Texture::Texture(const char* texturePath, GLenum type, GLenum slot, GLenum format, GLenum pixelType){
    // Load the player's texture

    // Texture Variables
    m_Type = type;
    int iPlayerImgWidth, iPlayerImgHeight, iPlayerImgChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* playerImgData;
    playerImgData = stbi_load(texturePath, &iPlayerImgWidth, &iPlayerImgHeight, &iPlayerImgChannels, 0);

    if (!playerImgData) {
        std::cerr << "Failed to load texture at " << texturePath << "\n";
    }
    
    glGenTextures(1, &m_ID);
    glActiveTexture(slot);
    glBindTexture(type, m_ID);

    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(type, 0, GL_RGBA, iPlayerImgWidth, iPlayerImgHeight, 0, format, pixelType, playerImgData);
    glGenerateMipmap(type);

    stbi_image_free(playerImgData);
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
    glDeleteTextures(1, &m_ID);
}
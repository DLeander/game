#include "texture.h"

Texture::Texture(const char* texturePath, GLenum type, GLenum slot, GLenum format, GLenum pixelType){
    // Load the player's texture

    // Texture Variables
    this->type = type;
    int playerImgWidth, playerImgHeight, playerImgChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* playerImgData;
    playerImgData = stbi_load(texturePath, &playerImgWidth, &playerImgHeight, &playerImgChannels, 0);

    if (!playerImgData) {
        std::cerr << "Failed to load texture at " << texturePath << "\n";
    }
    
    glGenTextures(1, &ID);
    glActiveTexture(slot);
    glBindTexture(type, ID);

    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(type, 0, GL_RGBA, playerImgWidth, playerImgHeight, 0, format, pixelType, playerImgData);
    glGenerateMipmap(type);

    stbi_image_free(playerImgData);
    glBindTexture(type, 0);
}

void Texture::textureUnit(Shader* shader, const char* uniform, GLuint unit){
    shader->Activate();
    glUniform1i(glGetUniformLocation(shader->ID, uniform), unit);
}

void Texture::Bind(){
    glBindTexture(type, ID);
}

void Texture::Unbind(){
    glBindTexture(type, 0);
}

void Texture::Delete(){
    glDeleteTextures(1, &ID);
}
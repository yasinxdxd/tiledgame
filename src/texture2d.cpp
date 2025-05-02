#define STB_IMAGE_IMPLEMENTATION
#include <texture2d.hpp>
#include <stb_image.h>
#include <glad/glad.h>
#include <stdio.h>
#include <iostream>

Texture2D::Texture2D(): m_texture2d_data(nullptr)
{

}

Texture2D::Texture2D(const char *path)
{
    load_texture(path);
}

Texture2D::Texture2D(int width, int height, unsigned char *data)
{
    create_texture(width, height, data);
}

Texture2D::~Texture2D()
{
    delete_texture();
    if (data_loaded_by_load_texture)
        stbi_image_free(m_texture2d_data);
}

bool Texture2D::load_texture(const char* path)
{
    data_loaded_by_load_texture = true;
    m_texture2d_data = stbi_load(path, &m_texture2d_width, &m_texture2d_height, &m_texture2d_nmbrof_clr_chnnels, 0);
    
    if(m_texture2d_data == nullptr)
        return false;
    
    return true;
}

void Texture2D::create_texture(int width, int height, unsigned char* data, int color_channels)
{
    m_texture2d_data = data;
    m_texture2d_width = width;
    m_texture2d_height = height;
    m_texture2d_nmbrof_clr_chnnels = color_channels;
}

void Texture2D::generate_texture()
{
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "Texture2D() generate begining error: " << error << std::endl;
    }
    //create and bind texture id
    glGenTextures(1, &m_gl_texture2d_texture_id);
    bind();

    //parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR_MIPMAP_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //generate mipmap
    switch (m_texture2d_nmbrof_clr_chnnels)
    {
    case 1:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_texture2d_width, m_texture2d_height, 0, GL_RED, GL_UNSIGNED_BYTE, m_texture2d_data);
        break;
    case 3:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texture2d_width, m_texture2d_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_texture2d_data);
        break;
    case 4:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_texture2d_width, m_texture2d_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_texture2d_data);
        break;
    default:
        break;
    }
    //glBindTextureUnit
    glGenerateMipmap(GL_TEXTURE_2D);
    unbind();
    error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "Texture2D() generate errorrrR!: " << error << std::endl;
    }
}

void Texture2D::delete_texture()
{
    glDeleteTextures(1, &m_gl_texture2d_texture_id);
}

void Texture2D::bind()
{
    glBindTexture(GL_TEXTURE_2D, m_gl_texture2d_texture_id);
}

void Texture2D::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}


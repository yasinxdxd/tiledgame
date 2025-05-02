#include <render_texture2d.hpp>
#include <texture2d.hpp>
#include <glad/glad.h>
#include <stdio.h>

RenderTexture2D::RenderTexture2D()
{
    
}

RenderTexture2D::RenderTexture2D(Texture2D* texture2d)
{
    set_texture(texture2d);
}

RenderTexture2D::~RenderTexture2D()
{
    glDeleteRenderbuffers(1, &m_gl_RBO);
    glDeleteFramebuffers(1, &m_gl_FBO);
    //we give the address of the pointer so we are not allowed to delete it, find a way to understand the parameter is a real pointer or an address of an non-heap variable..
    //if(m_texture2d)
        //delete m_texture2d;
}

void RenderTexture2D::set_texture(Texture2D *texture2d)
{
    m_texture2d = texture2d;

    glGenFramebuffers(1, &m_gl_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_gl_FBO);
    m_texture2d->generate_texture();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *m_texture2d, 0);

    
    glGenRenderbuffers(1, &m_gl_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_gl_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 
        m_texture2d->m_texture2d_width, m_texture2d->m_texture2d_height); // use a single renderbuffer object for both a depth AND stencil buffer.
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_gl_RBO);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	    printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");

    //glBindFramebuffer(GL_FRAMEBUFFER, 0); 
}

void RenderTexture2D::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_gl_FBO);
    glEnable(GL_DEPTH_TEST);
}

void RenderTexture2D::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
}

void RenderTexture2D::bind_texture()
{
    m_texture2d->bind();
}

void RenderTexture2D::unbind_texture()
{
    m_texture2d->unbind();
}

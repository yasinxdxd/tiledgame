#ifndef RENDER_TEXTURE2D_HPP
#define RENDER_TEXTURE2D_HPP

class Texture2D;

class RenderTexture2D
{
public:
    RenderTexture2D();
    RenderTexture2D(Texture2D* texture2d);
    ~RenderTexture2D();


    Texture2D* get_texture() { return m_texture2d; };
    void set_texture(Texture2D* texture2d);

    void bind();
    void unbind();
    void bind_texture();
    void unbind_texture();

private:
    unsigned int m_gl_FBO;//frame buffer object
    unsigned int m_gl_RBO;//render buffer object
    unsigned int m_tt;
    Texture2D* m_texture2d;
};

#endif//RENDER_TEXTURE2D_HPP
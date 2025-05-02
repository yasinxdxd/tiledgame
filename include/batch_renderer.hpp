#ifndef BATCH_RENDERER_HPP
#define BATCH_RENDERER_HPP

#include <Definitions.hh>
#include <defs.hh>

#define SHAPE_POINTS 0x0000
#define SHAPE_LINES 0x0001
#define SHAPE_LINE_LOOP 0x0002
#define SHAPE_LINE_STRIP 0x0003
#define SHAPE_TRIANGLES 0x0004
#define SHAPE_TRIANGLE_STRIP 0x0005
#define SHAPE_TRIANGLE_FAN 0x0006

class BatchRenderer
{
public:
    BatchRenderer(Shader* shader);
    ~BatchRenderer();

    void begin();
    void set_shader(shader_function_capture func);
    void vertex(float px, float py, float pz, float u, float v, float r, float g, float b);
    void end();
    
    void render(unsigned int primary = SHAPE_TRIANGLES);


private:
    void init();
    void init_buffers();
    void init_shader();
    void destroy();

private:

    u32 m_gl_VBO;//vertex buffer object
    u32 m_gl_EBO;//vertex buffer object
    u32 m_gl_VAO;//vertex array object
    Shader* m_batch_shader;
    shader_function_batch m_shader_func;

    static constexpr u32 MAX_VERT_COUNT = 1024;
    static constexpr u32 MAX_IND_COUNT = MAX_VERT_COUNT;
    unsigned int m_vertices_index = 0;
    unsigned int m_indices_index = 0;
    float m_vertices[MAX_VERT_COUNT] = {0};
    u32 m_indices[MAX_VERT_COUNT] = {0};
};

#endif//BATCH_RENDERER_HPP
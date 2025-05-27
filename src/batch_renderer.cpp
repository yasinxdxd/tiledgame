#include <iostream>
#include <batch_renderer.hpp>
#include <glad/glad.h>
#include <defs.hh>
#include <shader.hpp>

BatchRenderer::BatchRenderer(Shader* shader)
{
    m_batch_shader = shader;
    init();
}

BatchRenderer::~BatchRenderer()
{
    destroy();
}

void BatchRenderer::init()
{
    init_buffers();
    init_shader();
}

void BatchRenderer::begin()
{
    m_vertices_index = 0;
    m_indices_index = 0;
}

void BatchRenderer::set_shader(shader_function_capture func)
{
    glUseProgram(*m_batch_shader);
    //m_batch_shader->set<float, 2>("u_window_size", width, height);
    if (func != NULL)
        func(m_batch_shader);
    //shader things...
}

void BatchRenderer::vertex(float px, float py, float pz, float u, float v, float r, float g, float b)
{
    assert(m_vertices_index < MAX_VERT_COUNT);

    m_vertices[m_vertices_index++] = px;
    m_vertices[m_vertices_index++] = py;
    m_vertices[m_vertices_index++] = pz;
    m_vertices[m_vertices_index++] = u;
    m_vertices[m_vertices_index++] = v;
    m_vertices[m_vertices_index++] = r;
    m_vertices[m_vertices_index++] = g;
    m_vertices[m_vertices_index++] = b;
}

void BatchRenderer::end()
{
    const u32 div = m_vertices_index / 32;
    for (size_t i = 0; i < div; i++)
    {
        u32 index_place = i * 4;
        m_indices[m_indices_index++] = 0 + index_place;
        m_indices[m_indices_index++] = 1 + index_place;
        m_indices[m_indices_index++] = 3 + index_place;
        m_indices[m_indices_index++] = 1 + index_place;
        m_indices[m_indices_index++] = 2 + index_place;
        m_indices[m_indices_index++] = 3 + index_place;
    }

    glBindVertexArray(m_gl_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_gl_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices_index * sizeof(m_vertices[0]), m_vertices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_indices_index * sizeof(m_indices[0]), m_indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void BatchRenderer::render(unsigned int primary)
{
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "BatchRenderer() BEFORE render error: " << error << std::endl;
    }
    glBindVertexArray(m_gl_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_gl_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_EBO);
    glDrawElements(primary, m_indices_index, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);//be sure that binded VAO is not changed by outside.
    glUseProgram(0);
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "BatchRenderer() AFTER render error: " << error << std::endl;
    }
}

void BatchRenderer::destroy()
{
    glDeleteBuffers(1, &m_gl_EBO);
    glDeleteBuffers(1, &m_gl_VBO);
    glDeleteVertexArrays(1, &m_gl_VAO);
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "BatchRenderer() destroy error! : " << error << std::endl;
    }
}

void BatchRenderer::init_buffers()
{
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "BatchRenderer::init(): error before init buffers: " << error << std::endl;
    }

    //vertex array object
    glGenVertexArrays(1, &m_gl_VAO);
    glBindVertexArray(m_gl_VAO);
    
    //vertex buffer object
    glGenBuffers(1, &m_gl_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_gl_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &m_gl_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices), m_indices, GL_DYNAMIC_DRAW);

    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "BatchRenderer::init(): gl buffer error: " << error << std::endl;
    }

    //position data
    glEnableVertexAttribArray(ATTRIB::POSITION);
    glVertexAttribPointer(ATTRIB::POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(m_vertices[0]), (void*)0);

    glEnableVertexAttribArray(ATTRIB::UV);
    glVertexAttribPointer(ATTRIB::UV, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(m_vertices[0]), (void*)(3* sizeof(float)));

    glEnableVertexAttribArray(ATTRIB::COLOR);
    glVertexAttribPointer(ATTRIB::COLOR, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(m_vertices[0]), (void*)(5* sizeof(float)));

    /*****************/
    /*
    GLuint instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 1024, nullptr, GL_DYNAMIC_DRAW);

    // Set up the instanced attribute pointer for shape positions
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr); // The instance attribute is bound to the separate instanceVBO
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);*/
    /*****************/

    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "BatchREnderer::init(): vertex attrib pointer error: " << error << std::endl;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // TODO: find a better way to enable and disable it!
    glEnable(GL_DEPTH_TEST);
}

#define ROOT_DIR "../../../pyopengui_lib"

void BatchRenderer::init_shader()
{
    if (glcompiler::is_initialized())
        glcompiler::compile_and_attach_shaders(m_batch_shader);
    else {
        glcompiler::init();
        glcompiler::compile_and_attach_shaders(m_batch_shader);
    }
}

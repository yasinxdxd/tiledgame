#ifndef RENDERABLES_HPP
#define RENDERABLES_HPP

#include <defs.hh>

#define SHAPE_POINTS 0x0000
#define SHAPE_LINES 0x0001
#define SHAPE_LINE_LOOP 0x0002
#define SHAPE_LINE_STRIP 0x0003
#define SHAPE_TRIANGLES 0x0004
#define SHAPE_TRIANGLE_STRIP 0x0005
#define SHAPE_TRIANGLE_FAN 0x0006

#ifdef SHAPE_QUAD

class Shader;

struct Quad
{
    Quad();
    ~Quad();

    operator unsigned int&() { return m_gl_VAO; }

private:
    float m_vertices[48] = {
    // positions        // texture coords  // colors
     1.f,  1.f, 0.0f,   1.0f, 1.0f,        1.0f, 1.0f, 1.0f, // top right
     1.f, -1.f, 0.0f,   1.0f, 0.0f,        1.0f, 1.0f, 1.0f, // bottom right
    -1.f, -1.f, 0.0f,   0.0f, 0.0f,        1.0f, 1.0f, 1.0f, // bottom left
    -1.f, -1.f, 0.0f,   0.0f, 0.0f,        1.0f, 1.0f, 1.0f, // bottom left
    -1.f,  1.f, 0.0f,   0.0f, 1.0f,        1.0f, 1.0f, 1.0f, // top left 
     1.f,  1.f, 0.0f,   1.0f, 1.0f,        1.0f, 1.0f, 1.0f, // top right
    };

    unsigned int m_gl_VBO;//vertex buffer object
    unsigned int m_gl_VAO;//vertex array object

};


#endif//SHAPE_QUAD

#ifdef SHAPE_CUBE

struct Cube
{
    Cube();
    ~Cube();

    operator unsigned int&() { return m_gl_VAO; }

private:
    float m_vertices[36 * 8] = {
        // positions         // tex coords // colors
        // Front face
        -1.f, -1.f,  1.f,    0.f, 0.f,     1.f, 1.f, 1.f,
         1.f, -1.f,  1.f,    1.f, 0.f,     1.f, 1.f, 1.f,
         1.f,  1.f,  1.f,    1.f, 1.f,     1.f, 1.f, 1.f,
         1.f,  1.f,  1.f,    1.f, 1.f,     1.f, 1.f, 1.f,
        -1.f,  1.f,  1.f,    0.f, 1.f,     1.f, 1.f, 1.f,
        -1.f, -1.f,  1.f,    0.f, 0.f,     1.f, 1.f, 1.f,

        // Back face
        -1.f, -1.f, -1.f,    1.f, 0.f,     1.f, 1.f, 1.f,
        -1.f,  1.f, -1.f,    1.f, 1.f,     1.f, 1.f, 1.f,
         1.f,  1.f, -1.f,    0.f, 1.f,     1.f, 1.f, 1.f,
         1.f,  1.f, -1.f,    0.f, 1.f,     1.f, 1.f, 1.f,
         1.f, -1.f, -1.f,    0.f, 0.f,     1.f, 1.f, 1.f,
        -1.f, -1.f, -1.f,    1.f, 0.f,     1.f, 1.f, 1.f,

        // Left face
        -1.f,  1.f,  1.f,    1.f, 0.f,     1.f, 1.f, 1.f,
        -1.f,  1.f, -1.f,    1.f, 1.f,     1.f, 1.f, 1.f,
        -1.f, -1.f, -1.f,    0.f, 1.f,     1.f, 1.f, 1.f,
        -1.f, -1.f, -1.f,    0.f, 1.f,     1.f, 1.f, 1.f,
        -1.f, -1.f,  1.f,    0.f, 0.f,     1.f, 1.f, 1.f,
        -1.f,  1.f,  1.f,    1.f, 0.f,     1.f, 1.f, 1.f,

        // Right face
         1.f,  1.f,  1.f,    0.f, 0.f,     1.f, 1.f, 1.f,
         1.f, -1.f, -1.f,    1.f, 1.f,     1.f, 1.f, 1.f,
         1.f,  1.f, -1.f,    1.f, 0.f,     1.f, 1.f, 1.f,
         1.f, -1.f, -1.f,    1.f, 1.f,     1.f, 1.f, 1.f,
         1.f,  1.f,  1.f,    0.f, 0.f,     1.f, 1.f, 1.f,
         1.f, -1.f,  1.f,    0.f, 1.f,     1.f, 1.f, 1.f,

        // Top face
        -1.f,  1.f, -1.f,    0.f, 1.f,     1.f, 1.f, 1.f,
        -1.f,  1.f,  1.f,    0.f, 0.f,     1.f, 1.f, 1.f,
         1.f,  1.f,  1.f,    1.f, 0.f,     1.f, 1.f, 1.f,
         1.f,  1.f,  1.f,    1.f, 0.f,     1.f, 1.f, 1.f,
         1.f,  1.f, -1.f,    1.f, 1.f,     1.f, 1.f, 1.f,
        -1.f,  1.f, -1.f,    0.f, 1.f,     1.f, 1.f, 1.f,

        // Bottom face
        -1.f, -1.f, -1.f,    1.f, 1.f,     1.f, 1.f, 1.f,
         1.f, -1.f,  1.f,    0.f, 0.f,     1.f, 1.f, 1.f,
        -1.f, -1.f,  1.f,    1.f, 0.f,     1.f, 1.f, 1.f,
         1.f, -1.f,  1.f,    0.f, 0.f,     1.f, 1.f, 1.f,
        -1.f, -1.f, -1.f,    1.f, 1.f,     1.f, 1.f, 1.f,
         1.f, -1.f, -1.f,    0.f, 1.f,     1.f, 1.f, 1.f
    };


    unsigned int m_gl_VBO;
    unsigned int m_gl_VAO;
};
#endif//SHAPE_CUBE


#ifdef SHAPE_TRIANGLE

struct Triangle
{
    Triangle();
    ~Triangle();

    operator unsigned int&() { return m_gl_VAO; }

private:
    float m_vertices[24] = {
    // positions        // texture coords   // colors      
    -1.f, -1.f, 0.0f,   0.0f, 0.0f,         1.0f, 0.0f, 0.0f,
     1.f, -1.f, 0.0f,   0.0f, 0.0f,         1.0f, 1.0f, 0.0f,
     0.0f, 1.f, 0.0f,   0.0f, 0.0f,         1.0f, 0.0f, 1.0f,
    };

    unsigned int m_gl_VBO;//vertex buffer object
    unsigned int m_gl_VAO;//vertex array object
};
#endif//SHAPE_TRIANGLE

#ifdef SHAPE_LINE

struct Line
{
    Line();
    ~Line();

    operator unsigned int&() { return m_gl_VAO; }

private:
    float m_vertices[16] = {
    // positions        // texture coords   // colors      
    -1.f, -1.f, 0.0f,   0.0f, 0.0f,         1.0f, 0.0f, 0.0f,
     1.f,  1.f, 0.0f,   0.0f, 0.0f,         1.0f, 1.0f, 0.0f,
    };

    unsigned int m_gl_VBO;//vertex buffer object
    unsigned int m_gl_VAO;//vertex array object
};
#endif//SHAPE_LINE

// TODO: do something about mesh class and quads and triangles...


void render(unsigned int& vertex_array_object, unsigned int size, Shader* shader, shader_function_capture func, unsigned int primary = SHAPE_TRIANGLES, bool use_ebo = false);


struct MeshStatic
{
    MeshStatic();
    ~MeshStatic();

    operator unsigned int&() { return m_gl_VAO; }

    void load_mesh_obj(const char *path, size_t& vertices_size, size_t& indices_size);
    void load_mesh_gltf(const char* path);

protected:
    float* m_vertices;
    unsigned int* m_indices;

    unsigned int m_gl_VBO;//vertex buffer object
    unsigned int m_gl_EBO;//index buffer object
    unsigned int m_gl_VAO;//vertex array object
};

struct MeshSkeletal : MeshStatic
{
    MeshSkeletal();
    virtual ~MeshSkeletal();

    operator unsigned int&() { return m_gl_VAO; }

    void load_mesh_obj(const char *path, size_t& vertices_size, size_t& indices_size);
    // void load_mesh_gltf(const char* path);

// extends...
// private: 
    // float* m_vertices;
    // unsigned int* m_indices;

    // unsigned int m_gl_VBO;//vertex buffer object
    // unsigned int m_gl_EBO;//index buffer object
    // unsigned int m_gl_VAO;//vertex array object
};

#endif//RENDERABLES_HPP
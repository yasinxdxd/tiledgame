// model loading libs
#include <OBJ_Loader.h>
//#include <my_obj_loader.hpp>
#define TINYGLTF_IMPLEMENTATION
// #define STB_IMAGE_IMPLEMENTATION // implementation is already defined in texture2d.cpp file
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

#define SHAPE_QUAD
#define SHAPE_CUBE
#define SHAPE_TRIANGLE
#define SHAPE_LINE
#include <renderables.hpp>
#include <shader.hpp>
#include <glad/glad.h>
#include <defs.hh>

#include <iostream>

Quad::Quad()
{
    //vertex array object
    glGenVertexArrays(1, &m_gl_VAO);
    glBindVertexArray(m_gl_VAO);
    
    //vertex buffer object
    glGenBuffers(1, &m_gl_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_gl_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);
    
    // TODO: use this shit below to set vertices buffer!
    //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_vertices), m_vertices);
    
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "gl buffer error: " << error << std::endl;
    }

    //position data
    glVertexAttribPointer(ATTRIB::POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(ATTRIB::POSITION);
    glVertexAttribPointer(ATTRIB::UV, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(ATTRIB::UV);
    glVertexAttribPointer(ATTRIB::COLOR, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5* sizeof(float)));
    glEnableVertexAttribArray(ATTRIB::COLOR);

    //delete
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "vertex attrib pointer error: " << error << std::endl;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Quad::~Quad()
{
    glDeleteBuffers(1, &m_gl_VBO);
    glDeleteVertexArrays(1, &m_gl_VAO);
}


Cube::Cube()
{
    //vertex array object
    glGenVertexArrays(1, &m_gl_VAO);
    glBindVertexArray(m_gl_VAO);
    
    //vertex buffer object
    glGenBuffers(1, &m_gl_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_gl_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);
    
    // TODO: use this shit below to set vertices buffer!
    //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_vertices), m_vertices);
    
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "gl buffer error: " << error << std::endl;
    }

    //position data
    glVertexAttribPointer(ATTRIB::POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(ATTRIB::POSITION);
    glVertexAttribPointer(ATTRIB::UV, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(ATTRIB::UV);
    glVertexAttribPointer(ATTRIB::COLOR, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5* sizeof(float)));
    glEnableVertexAttribArray(ATTRIB::COLOR);

    //delete
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "vertex attrib pointer error: " << error << std::endl;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Cube::~Cube()
{
    glDeleteBuffers(1, &m_gl_VBO);
    glDeleteVertexArrays(1, &m_gl_VAO);
}

Triangle::Triangle()
{
    //vertex array object
    glGenVertexArrays(1, &m_gl_VAO);
    glBindVertexArray(m_gl_VAO);
    
    //vertex buffer object
    glGenBuffers(1, &m_gl_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_gl_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);
    

    //position data
    glVertexAttribPointer(ATTRIB::POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(ATTRIB::POSITION);
    glVertexAttribPointer(ATTRIB::UV, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(ATTRIB::UV);
    glVertexAttribPointer(ATTRIB::COLOR, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5* sizeof(float)));
    glEnableVertexAttribArray(ATTRIB::COLOR);

    //delete
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Line::~Line()
{
    glDeleteBuffers(1, &m_gl_VBO);
    glDeleteVertexArrays(1, &m_gl_VAO);
}

Line::Line()
{
    //vertex array object
    glGenVertexArrays(1, &m_gl_VAO);
    glBindVertexArray(m_gl_VAO);
    
    //vertex buffer object
    glGenBuffers(1, &m_gl_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_gl_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);
    

    //position data
    glVertexAttribPointer(ATTRIB::POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(ATTRIB::POSITION);
    glVertexAttribPointer(ATTRIB::UV, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(ATTRIB::UV);
    glVertexAttribPointer(ATTRIB::COLOR, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5* sizeof(float)));
    glEnableVertexAttribArray(ATTRIB::COLOR);

    //delete
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Triangle::~Triangle()
{
    glDeleteBuffers(1, &m_gl_VBO);
    glDeleteVertexArrays(1, &m_gl_VAO);
}

void render(unsigned int& vertex_array_object, unsigned int size, Shader* shader, shader_function_capture func, unsigned int primary, bool use_ebo)
{
    GLenum error;
    glUseProgram(*shader);
    if (func)
        func(shader);

    glBindVertexArray(vertex_array_object);

    if(!use_ebo) {
        glDrawArrays(primary, 0, size);
        while ((error = glGetError()) != GL_NO_ERROR) {
            std::cout << "glDrawArrays error: " << error << std::endl;
        }
    } else {
        glDrawElements(primary, size, GL_UNSIGNED_INT, 0);
        while ((error = glGetError()) != GL_NO_ERROR) {
            std::cout << "glDrawElements error: " << error << std::endl;
        }
    }
    glBindVertexArray(NULL);//be sure that binded VAO is not changed by outside.
    glUseProgram(NULL);
    
}

MeshStatic::MeshStatic()
{
    m_vertices = nullptr;
    m_indices = nullptr;
    glGenVertexArrays(1, &m_gl_VAO);
    glBindVertexArray(m_gl_VAO);
    //vertex buffer object
    glGenBuffers(1, &m_gl_VBO);
    //glBindBuffer()
    // FIXME: add index buffer and render mesh through indicies...
    glGenBuffers(1, &m_gl_EBO);
}

MeshStatic::~MeshStatic()
{
    glDeleteBuffers(1, &m_gl_VBO);
    glDeleteBuffers(1, &m_gl_EBO);
    glDeleteVertexArrays(1, &m_gl_VAO);

    if (m_vertices)
        delete[] m_vertices;
    if (m_indices)
        delete[] m_indices;
}

void MeshStatic::load_mesh_obj(const char *path, size_t& vertices_size, size_t& indices_size)
{
    // Initialize OBJ Loader
    objl::Loader Loader;
	// Load .obj File
	
    //std::cout << "meshes count: " << "" << std::endl;
    if (Loader.LoadFile(path))
	{
        std::cout << "meshes count: " << Loader.LoadedMeshes.size() << std::endl;
        vertices_size = 0;
        indices_size = 0;
		// Go through each loaded mesh and out its contents
		for (size_t m = 0; m < Loader.LoadedMeshes.size(); m++)
        {
            objl::Mesh curMesh = Loader.LoadedMeshes[m];
            vertices_size += curMesh.Vertices.size();
            indices_size += curMesh.Indices.size();

        }
        const size_t _vertices_size = vertices_size * 8;
        const size_t _indices_size = indices_size;

        m_vertices = new float[_vertices_size];
        m_indices = new unsigned int[_indices_size];

        size_t vert_counter = 0;
        size_t ind_counter = 0;
        //size_t j = 0;
        for (size_t m = 0; m < Loader.LoadedMeshes.size(); m++)
        {
            objl::Mesh curMesh = Loader.LoadedMeshes[m];
            for (size_t i = vert_counter; i < vert_counter + curMesh.Vertices.size(); i++)
            {
                m_vertices[i*8 + 0] = curMesh.Vertices[i - vert_counter].Position.X;
                m_vertices[i*8 + 1] = curMesh.Vertices[i - vert_counter].Position.Y;
                m_vertices[i*8 + 2] = curMesh.Vertices[i - vert_counter].Position.Z;
                m_vertices[i*8 + 3] = curMesh.Vertices[i - vert_counter].TextureCoordinate.X;
                m_vertices[i*8 + 4] = curMesh.Vertices[i - vert_counter].TextureCoordinate.Y;
                m_vertices[i*8 + 5] = curMesh.Vertices[i - vert_counter].Normal.X;
                m_vertices[i*8 + 6] = curMesh.Vertices[i - vert_counter].Normal.Y;
                m_vertices[i*8 + 7] = curMesh.Vertices[i - vert_counter].Normal.Z;
                //j += 8;
            }
            for (size_t i = ind_counter; i < ind_counter + curMesh.Indices.size(); i++)
            {
                m_indices[i] = curMesh.Indices.at(i - ind_counter);
            }
            vert_counter += curMesh.Vertices.size();
            ind_counter += curMesh.Indices.size();
            std::cout << "ind: " << ind_counter << " vert: " << vert_counter << std::endl;
        }

        
        glBindVertexArray(m_gl_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_gl_VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_EBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _vertices_size, m_vertices, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _indices_size, m_indices, GL_STATIC_DRAW);

        //position data
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5* sizeof(float)));
        glEnableVertexAttribArray(2);

        //delete
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

void MeshStatic::load_mesh_gltf(const char *path)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;
    std::string file_path(path);

    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, file_path);
    //bool ret = loader.LoadBinaryFromFile(&scene, &err, file_path); // for binary glTF(.glb)

    if (!warn.empty())
    {
        printf("Warn: %s\n", warn.c_str());
    }

    if (!err.empty())
    {
        printf("Err: %s\n", err.c_str());
        return;
    }

    if (!ret)
    {
        printf("Failed to parse glTF\n");
        return;
    }

    std::cout << std::endl;
    std::cout << "gltf loader::model_mesh_size:" << model.meshes.size() << std::endl;

    model.buffers[0];//

    for (auto& mesh : model.meshes)
    {
        mesh;
    }
}

MeshSkeletal::MeshSkeletal()
{
    MeshStatic();
}

MeshSkeletal::~MeshSkeletal()
{
    ~MeshStatic();
}

void MeshSkeletal::load_mesh_obj(const char *path, size_t &vertices_size, size_t &indices_size)
{
    MeshStatic::load_mesh_obj(path, vertices_size, indices_size);

    glBindVertexArray(m_gl_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_gl_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_EBO);

    //position data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(5* sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(3, 3, GL_INT, 11 * sizeof(float), (void*)(3 * sizeof(int)));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

#include <shader.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <defs.hh>
#include <glad/glad.h>
#include <stdarg.h>

//extern inline glshader_t _shader_DEFAULT_GL_VERTEX_SHADER;

Shader::Shader():
m_shader_fragment_code_text(nullptr),
m_shader_geometry_code_text(nullptr),
m_shader_vertex_code_text(nullptr)
{
    m_use_default_vertex_shader = true;
}

Shader::Shader(const char* path, ShaderCodeType type):
m_shader_fragment_code_text(nullptr),
m_shader_geometry_code_text(nullptr),
m_shader_vertex_code_text(nullptr)
{
    m_use_default_vertex_shader = true;
    load_shader_code(path, type);
}

Shader::~Shader()
{
    //we have deleted them inside the shader_compiler.cpp
    /*
    if(m_shader_fragment_code_text)
        delete m_shader_fragment_code_text;
    if(m_shader_vertex_code_text)
        delete m_shader_vertex_code_text;
    */
    glDeleteProgram(m_gl_shader_program_id);
}

void Shader::load_shader_code(const char* path, ShaderCodeType type)
{
    std::ifstream shader_file;   
    shader_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    std::cout << path << std::endl;
    std::string** shader_text_referance;

    switch (type)
    {
    case ShaderCodeType::VERTEX_SHADER:
    {
        shader_text_referance = &m_shader_vertex_code_text;
        m_use_default_vertex_shader = false;
        break;
    }
    case ShaderCodeType::GEOMETRY_SHADER:
    {
        shader_text_referance = &m_shader_geometry_code_text;
        // it is not that possible to use default vertex shader if you use a custom gemoetry shader.
        m_use_default_vertex_shader = false;
        break;
    }
    case ShaderCodeType::FRAGMENT_SHADER:
    {
        shader_text_referance = &m_shader_fragment_code_text;
        break;
    }
    default:
        break;
    }
    
    try 
    {
        shader_file.open(path);

        if (shader_file.is_open())
        {
            std::stringstream Shader_sstream;
            // read file's buffer contents into stream
            Shader_sstream << shader_file.rdbuf();
            // close file handler
            shader_file.close();
            // convert stream into string
            if(*shader_text_referance == nullptr)
            {
                *shader_text_referance = new std::string(Shader_sstream.str());
            }
            else
            {
                delete shader_text_referance;
                *shader_text_referance = new std::string(Shader_sstream.str());
            }
        }
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << 
        e.what() << 
        std::endl;
    }
    
}

Shader::operator unsigned int()
{
    return m_gl_shader_program_id;
}

// FIXME: FIX the multi dimensional set methods with a better structure!!!

template <>
void Shader::set<int>(std::string name, int value) const
{
    glUseProgram(m_gl_shader_program_id);

    glUniform1i(
        glGetUniformLocation(m_gl_shader_program_id, name.c_str()),
        value);
}

template <>
void Shader::set<unsigned int>(std::string name, unsigned int value) const
{
    glUseProgram(m_gl_shader_program_id);

    glUniform1ui(
        glGetUniformLocation(m_gl_shader_program_id, name.c_str()),
        value);
}

template <>
void Shader::set<float>(std::string name, float value) const
{
    glUseProgram(m_gl_shader_program_id);

    glUniform1f(
        glGetUniformLocation(m_gl_shader_program_id, name.c_str()),
        value);
}

template <>
void Shader::set<double>(std::string name, double value) const
{
    glUseProgram(m_gl_shader_program_id);

    glUniform1d(
        glGetUniformLocation(m_gl_shader_program_id, name.c_str()),
        value);
}

template <>
void Shader::set<int, 2>(std::string name, int value1, int value2) const
{
    glUseProgram(m_gl_shader_program_id);

    glUniform2i(
        glGetUniformLocation(m_gl_shader_program_id, name.c_str()),
        value1, value2);
}

template <>
void Shader::set<unsigned int, 2>(std::string name, unsigned int value1, unsigned int value2) const
{
    glUseProgram(m_gl_shader_program_id);

    glUniform2ui(
        glGetUniformLocation(m_gl_shader_program_id, name.c_str()),
        value1, value2);
}

template <>
void Shader::set<float, 2>(std::string name, float value1, float value2) const
{
    glUseProgram(m_gl_shader_program_id);

    glUniform2f(
        glGetUniformLocation(m_gl_shader_program_id, name.c_str()),
        value1, value2);
}

template <>
void Shader::set<double, 2>(std::string name, double value1, double value2) const
{
    glUseProgram(m_gl_shader_program_id);

    glUniform2d(
        glGetUniformLocation(m_gl_shader_program_id, name.c_str()),
        value1, value2);
}

template <>
void Shader::set<int, 3>(std::string name, int value1, int value2, int value3) const
{
    glUseProgram(m_gl_shader_program_id);

    glUniform3i(
        glGetUniformLocation(m_gl_shader_program_id, name.c_str()),
        value1, value2, value3);
}

template <>
void Shader::set<unsigned int, 3>(std::string name, unsigned int value1, unsigned int value2, unsigned int value3) const
{
    glUseProgram(m_gl_shader_program_id);

    glUniform3ui(
        glGetUniformLocation(m_gl_shader_program_id, name.c_str()),
        value1, value2, value3);
}

template <>
void Shader::set<float, 3>(std::string name, float value1, float value2, float value3) const
{
    glUseProgram(m_gl_shader_program_id);

    glUniform3f(
        glGetUniformLocation(m_gl_shader_program_id, name.c_str()),
        value1, value2, value3);
}

template <>
void Shader::set<double, 3>(std::string name, double value1, double value2, double value3) const
{
    glUseProgram(m_gl_shader_program_id);

    glUniform3d(
        glGetUniformLocation(m_gl_shader_program_id, name.c_str()),
        value1, value2, value3);
}

template <>
void Shader::set<int, 4>(std::string name, int value1, int value2, int value3, int value4) const
{
    glUseProgram(m_gl_shader_program_id);

    glUniform4i(
        glGetUniformLocation(m_gl_shader_program_id, name.c_str()),
        value1, value2, value3, value4);
}

template <>
void Shader::set<unsigned int, 4>(std::string name, unsigned int value1, unsigned int value2, unsigned int value3, unsigned int value4) const
{
    glUseProgram(m_gl_shader_program_id);

    glUniform4ui(
        glGetUniformLocation(m_gl_shader_program_id, name.c_str()),
        value1, value2, value3, value4);
}

template <>
void Shader::set<float, 4>(std::string name, float value1, float value2, float value3, float value4) const
{
    glUseProgram(m_gl_shader_program_id);

    glUniform4f(
        glGetUniformLocation(m_gl_shader_program_id, name.c_str()),
        value1, value2, value3, value4);
}

template <>
void Shader::set<double, 4>(std::string name, double value1, double value2, double value3, double value4) const
{
    glUseProgram(m_gl_shader_program_id);

    glUniform4d(
        glGetUniformLocation(m_gl_shader_program_id, name.c_str()),
        value1, value2, value3, value4);
}

template <>
void Shader::set_matrix<2>(std::string name, glm::mat2x2 value) const
{
    glUseProgram(m_gl_shader_program_id);

    glUniformMatrix2fv(
        glGetUniformLocation(m_gl_shader_program_id, name.c_str()),
        1, GL_FALSE, glm::value_ptr(value));
}

template <>
void Shader::set_matrix<3>(std::string name, glm::mat3x3 value) const
{
    glUseProgram(m_gl_shader_program_id);

    glUniformMatrix3fv(
        glGetUniformLocation(m_gl_shader_program_id, name.c_str()),
        1, GL_FALSE, glm::value_ptr(value));
}

template <>
void Shader::set_matrix<4>(std::string name, glm::mat4x4 value) const
{
    glUseProgram(m_gl_shader_program_id);

    glUniformMatrix4fv(
        glGetUniformLocation(m_gl_shader_program_id, name.c_str()),
        1, GL_FALSE, glm::value_ptr(value));
}

/*
    glUniformMatrix2dv
    glUniformMatrix2fv
    glUniformMatrix3dv
    glUniformMatrix3fv
    glUniformMatrix4dv
    glUniformMatrix4fv
*/

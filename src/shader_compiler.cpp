//#include <shader_compiler.hpp>
#include <shader.hpp>
#include <glad/glad.h>
#include <iostream>
#include "shader_compiler.hpp"

extern const char* _SHADER_DEFAULT_VERTEX_SHADER_CODE_TEXT;
extern glshader_t _SHADER_DEFAULT_GL_VERTEX_SHADER;

glcompiler::_priv::_compiler_info compiler_info;

void glcompiler::init()
{
    _priv::compile_default_vetex_shader();
    _priv::is_compiler_initialized = true;
}

bool glcompiler::is_initialized()
{
    return _priv::is_compiler_initialized;
}
void glcompiler::destroy()
{
    if (is_initialized())
        glDeleteShader(_SHADER_DEFAULT_GL_VERTEX_SHADER);
}
void glcompiler::compile_and_attach_shaders(Shader *shader)
{
    glshader_t fragment_shader;
    glshader_t vertex_shader;
    glshader_t geometry_shader;
    int success;
    char info_log[512];
    GLenum error;
    
    // shader Program
    shader->m_gl_shader_program_id = glCreateProgram();


    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* _fragment_code = shader->m_shader_fragment_code_text->c_str();
    glShaderSource(fragment_shader, 1, &_fragment_code, NULL);
    glCompileShader(fragment_shader);
    
    // print compile errors if any
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
        memcpy(compiler_info.compile_log_info, info_log, 512);
    }
    else
        memset(compiler_info.compile_log_info, 0, 512);
    
    if(shader->m_use_default_vertex_shader == false)
    {
        const char* _vertex_code;
        if (shader->m_shader_vertex_code_text->c_str() != nullptr)
            _vertex_code = shader->m_shader_vertex_code_text->c_str();
        else
        {
            std::cout << "ERROR::SHADER::VERTEX::(if you want to use a custom geometry shader, vertex shader must be written by you!)\n" << std::endl;
            return;
        }

        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &_vertex_code, NULL);
        glCompileShader(vertex_shader);

        // print compile errors if any
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
            memcpy(compiler_info.compile_log_info, info_log, 512);
        }
        else
            memset(compiler_info.compile_log_info, 0, 512);
    
    
        glAttachShader(shader->m_gl_shader_program_id, vertex_shader);
    }
    else
        glAttachShader(shader->m_gl_shader_program_id, _SHADER_DEFAULT_GL_VERTEX_SHADER);  

    if (shader->m_shader_geometry_code_text != nullptr)
    {
        const char* _geometry_code;
        _geometry_code = shader->m_shader_geometry_code_text->c_str();

        geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry_shader, 1, &_geometry_code, NULL);
        glCompileShader(geometry_shader);

        // print compile errors if any
        glGetShaderiv(geometry_shader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(geometry_shader, 512, NULL, info_log);
            std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << info_log << std::endl;
            memcpy(compiler_info.compile_log_info, info_log, 512);
        }
        else
            memset(compiler_info.compile_log_info, 0, 512);
        
        glAttachShader(shader->m_gl_shader_program_id, geometry_shader);
    }
    
    glAttachShader(shader->m_gl_shader_program_id, fragment_shader);
    
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "shader attachment error: " << error << std::endl;
    }

    //linking the program
    glLinkProgram(shader->m_gl_shader_program_id);
    // print linking errors if any
    glGetProgramiv(shader->m_gl_shader_program_id, GL_LINK_STATUS, &success);
    
    if(!success)
    {
        glGetProgramInfoLog(shader->m_gl_shader_program_id, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
        memcpy(compiler_info.compile_log_info, info_log, 512);
    }
    else
        memset(compiler_info.compile_log_info, 0, 512);


    // delete the shaders as they're linked into our program now and no longer necessary
    if(shader->m_use_default_vertex_shader == false)
        glDeleteShader(vertex_shader);

    //TODO: it causes to delete _SHADER_DEFAULT_GL_VERTEX_SHADER more than once, create a destroy function to delete this shader
    //glDeleteShader(_SHADER_DEFAULT_GL_VERTEX_SHADER);
    glDeleteShader(fragment_shader);

    delete shader->m_shader_fragment_code_text;
    delete shader->m_shader_vertex_code_text;
    shader->m_shader_fragment_code_text = nullptr;
    shader->m_shader_vertex_code_text = nullptr;
    if (shader->m_shader_geometry_code_text)
        delete shader->m_shader_geometry_code_text;
    shader->m_shader_geometry_code_text = nullptr;

    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "shader link error: " << error << std::endl;
    }
}

void glcompiler::_priv::compile_default_vetex_shader()
{
    //glshader_t default_vertex_shader;
    int success;
    char info_log[512];
    
    _SHADER_DEFAULT_GL_VERTEX_SHADER = glCreateShader(GL_VERTEX_SHADER);
    const char** _default_vertex_code = &_SHADER_DEFAULT_VERTEX_SHADER_CODE_TEXT;
    glShaderSource(_SHADER_DEFAULT_GL_VERTEX_SHADER, 1, _default_vertex_code, NULL);
    glCompileShader(_SHADER_DEFAULT_GL_VERTEX_SHADER);
    
    // print compile errors if any
    glGetShaderiv(_SHADER_DEFAULT_GL_VERTEX_SHADER, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(_SHADER_DEFAULT_GL_VERTEX_SHADER, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::DEFAULT_VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
        memcpy(compiler_info.compile_log_info, info_log, 512);
    }
    else
        memset(compiler_info.compile_log_info, 0, 512);
        
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "default vertex shader compile error: " << error << std::endl;
    }
}

void glcompiler::save_shader_binary()
{
    //TODO: fill it.
}
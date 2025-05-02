#ifndef Shader_HPP
#define Shader_HPP

#include <string>
#include <shader_compiler.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    enum class ShaderCodeType
    {
        VERTEX_SHADER,
        GEOMETRY_SHADER,
        FRAGMENT_SHADER
    };

public:
    Shader();
    Shader(const char* path, ShaderCodeType type = ShaderCodeType::FRAGMENT_SHADER);
    Shader(const Shader&) = delete; // non construction-copyable
    Shader& operator=(const Shader&) = delete; // non copyable
    ~Shader();

    void load_shader_code(const char* path, ShaderCodeType type = ShaderCodeType::FRAGMENT_SHADER);

    
    template <typename T, size_t _D = 1>
    void set(std::string name, T value) const;
    template <typename T, size_t _D = 2>
    void set(std::string name, T value1, T value2) const;
    template <typename T, size_t _D = 3>
    void set(std::string name, T value1, T value2, T value3) const;
    template <typename T, size_t _D = 4>
    void set(std::string name, T value1, T value2, T value3, T value4) const;
    template <size_t _D = 2>
    void set_matrix(std::string name, glm::mat2x2 value) const;
    template <size_t _D = 3>
    void set_matrix(std::string name, glm::mat3x3 value) const;
    template <size_t _D = 4>
    void set_matrix(std::string name, glm::mat4x4 value) const;

    operator unsigned int();

private:
    std::string* m_shader_vertex_code_text;
    std::string* m_shader_geometry_code_text;
    std::string* m_shader_fragment_code_text;
    unsigned int m_gl_shader_program_id;
    bool m_use_default_vertex_shader;

    friend void glcompiler::compile_and_attach_shaders(Shader* shader);
    friend void glcompiler::save_shader_binary();
};

#endif//Shader_HPP

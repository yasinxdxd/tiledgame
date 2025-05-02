#ifndef SHADER_COMPILER_HPP
#define SHADER_COMPILER_HPP

class Shader;

#include <defs.hh>

namespace glcompiler
{
    namespace _priv
    {
        _NORETURN_ void compile_default_vetex_shader();
        struct _compiler_info
        {
            char compile_log_info[512];
        };
        static inline bool is_compiler_initialized = false;
    }

    void init();
    bool is_initialized();
    void destroy();
    
    _NORETURN_ void compile_and_attach_shaders(Shader* shader);
    //FIXME: it will save shader binary into a file.
    _NORETURN_ void save_shader_binary();


}
#endif//SHADER_COMPILER_HPP
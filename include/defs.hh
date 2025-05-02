#ifndef DEFS_HH
#define DEFS_HH

typedef unsigned int glshader_t;
//#ifdef __EMSCRIPTEN__
typedef unsigned long long int __uint64;
//#endif

enum ATTRIB : __uint64
{
    POSITION = 0,
    UV,
    COLOR
};


#ifdef __EMSCRIPTEN__
#define _NORETURN_ 
#else
#define _NORETURN_ [[ noreturn ]]
#endif

#define PI 3.14159
#define RAD2DEG(x) (x * 180.0) / PI
#define DEG2RAD(x) (x * PI) / 180.0

#include <functional>
class Shader;

typedef void (*shader_function)(Shader*);
typedef std::function<void(Shader*)> shader_function_capture;
typedef void (*shader_function_batch)(Shader*);

#endif//DEFS_HH
#ifndef DEFAULTS_HPP
#define DEFAULTS_HPP

//typedefs
typedef unsigned int glshader_t;

/*--------------------global default variables--------------------*/

#if defined __EMSCRIPTEN__ || defined SDL_BACKEND
const char* _SHADER2D_DEFAULT_VERTEX_SHADER_CODE_TEXT = 
R"V0G0N(#version 300 es
in vec4 i_position;
in vec3 i_color;
in vec2 i_uv;

out vec3 o_vertex_color; // specify a color output to the fragment shader
out vec2 o_vertex_uv; // specify a color output to the fragment shader

void main(void) {
    gl_Position = vec4(i_position.xyz, 1.0);

    o_vertex_color = i_color;
    o_vertex_uv = i_uv;
}
)V0G0N";

#else 

const char* _SHADER_DEFAULT_VERTEX_SHADER_CODE_TEXT = 
R"V0G0N(
#version 330 core
layout (location = 0) in vec3 i_position; // the position variable has attribute position 0
layout (location = 1) in vec2 i_uv;
layout (location = 2) in vec3 i_color;

  
out vec3 o_vertex_color; // specify a color output to the fragment shader
out vec2 o_vertex_uv; // specify a color output to the fragment shader

void main()
{
    gl_Position = vec4(i_position, 1.0);
    
    //send to fragment shader
    o_vertex_color = i_color;
    o_vertex_uv = i_uv;
}
)V0G0N";

#endif

glshader_t _SHADER_DEFAULT_GL_VERTEX_SHADER;

/*--------------------global default variables--------------------*/

#endif//DEFAULTS_HPP
#version 430 core
layout (location = 0) in vec3 i_position; // the position variable has attribute position 0
layout (location = 1) in vec3 i_color;
layout (location = 2) in vec2 i_uv;

  
out vec3 o_vertex_color; // specify a color output to the fragment shader
out vec2 o_vertex_uv; // specify a color output to the fragment shader

void main()
{
    gl_Position = vec4(i_position, 1.0);
    
    //send to fragment shader
    o_vertex_color = i_color;
    o_vertex_uv = i_uv;
}
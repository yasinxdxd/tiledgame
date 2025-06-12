/*
BATCH TEXT RENDERER VERTEX SHADER
*/
#version 430 core
layout (location = 0) in vec3 i_position;//model coordinates
layout (location = 1) in vec2 i_uv;
layout (location = 2) in vec3 i_color;
/*-------*/
uniform vec2 u_window_size;
/*-------*/
out vec2 o_vertex_uv; // specify a uv output to the fragment shader
out vec3 o_vertex_color; // specify a color output to the fragment shader
/*-------*/


vec2 map_screen_coordinates(vec2 pos)
{
    vec2 result = (pos * 2) / (u_window_size) - 1.0;
    return vec2(result.x, result.y);
}

void main()
{
    vec2 position = map_screen_coordinates(i_position.xy);
    gl_Position = vec4(position, 1.0, 1.0);// TODO: i_position.z as z component...
    
    //send to fragment shader
    o_vertex_color = i_color;
    o_vertex_uv = i_uv;
}
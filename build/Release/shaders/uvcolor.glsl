#version 330 core

out vec4 result_frag_color;
in vec3 o_vertex_color;
in vec2 o_vertex_uv;

void main()
{
    vec2 uv = o_vertex_uv.xy;
    
    result_frag_color = vec4(1.0, 1.0, 0.0, 1.0);
}
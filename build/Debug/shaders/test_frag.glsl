#version 430 core

out vec4 result_frag_color;
in vec3 o_vertex_color;
in vec2 o_vertex_uv;

uniform sampler2D u_texture;

void main()
{
    //result_frag_color = vec4(o_vertex_uv.rg, 0.0, 1.0);
    result_frag_color = texture(u_texture, o_vertex_uv);
}
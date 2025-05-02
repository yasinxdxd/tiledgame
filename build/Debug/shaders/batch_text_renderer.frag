/*
BATCH TEXT RENDERER FRAGMENT SHADER
*/
#version 330 core

out vec4 result_frag_color;
in vec2 o_vertex_uv;
in vec3 o_vertex_color;

uniform sampler2D u_texture;
//uniform vec3 u_color;

void main()
{
    vec2 uv = vec2(o_vertex_uv.x, o_vertex_uv.y);
    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_texture, o_vertex_uv).g);
    result_frag_color = vec4(o_vertex_color, texture(u_texture, o_vertex_uv).a);//o_vertex_color

    // vec4 texture_atlas = texture(u_texture, uv);
    // result_frag_color = (vec4(texture_atlas.r, texture_atlas.r, texture_atlas.r, texture_atlas.a) * (texture_atlas.r)) * vec4(o_vertex_color, 1.0);
    //FIXME:!!

    // result_frag_color = vec4(o_vertex_color, 1.0);
}
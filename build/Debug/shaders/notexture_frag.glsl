#version 430 core

out vec4 result_frag_color;
in vec3 o_vertex_color;
in vec2 o_vertex_uv;


uniform vec2 mouse_pos;
uniform float time;


void main()
{
    // TODO: handle them via screen size uniform...
    vec2 mouse = vec2(mouse_pos.x/1920, mouse_pos.y/1080);
    vec2 uv = o_vertex_uv;

    if (uv.x - 0.01 < mouse.x && uv.x + 0.01 > mouse.x
        &&
        uv.y - 0.01 < 1 - mouse.y && uv.y + 0.01 > 1 - mouse.y)
        result_frag_color = vec4(1.0, 1.0, 0.9, 1.0);
    else
        result_frag_color = vec4(0, 0, 0, 1);
};

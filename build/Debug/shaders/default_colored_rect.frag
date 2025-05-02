#version 430 core

out vec4 result_frag_color;
in vec3 o_vertex_color;
in vec2 o_vertex_uv;

uniform vec3 u_color;

vec4 round_box(vec2 uv, float radius)
{
    float THRESHOLD = 0.0001;
    vec2 center = vec2(0.5, 0.5);
    if (length(max(abs(uv - center)-vec2(0.15 - radius),0.0)) - 0.3 < THRESHOLD)
    {
        return vec4(u_color, 1.0);
    }
    return vec4(0.0);
}

void main()
{
    vec2 uv = o_vertex_uv.xy;
    
    vec2 center = vec2(0.5f);
    float radius = 0.5f;
    if (length(center - uv) < radius)
    {
        result_frag_color = vec4(o_vertex_color, 1.0);
    }
    else
    {
        result_frag_color = vec4(o_vertex_color, 0.0);
    }
    
    //result_frag_color = vec4(o_vertex_color, 1.0);
}
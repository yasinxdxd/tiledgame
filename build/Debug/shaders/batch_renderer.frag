#version 330 core

out vec4 result_frag_color;
in vec3 o_vertex_color;
in vec2 o_vertex_uv;


uniform vec4 u_color;
uniform float u_progress;

void main()
{	
	// Simulate fake lighting using the UV's y coordinate (light from top)
    float lighting = 0.8 + 0.5 * o_vertex_uv.y; // Range from 0.5 to 1.0
    vec3 lit_color = u_color.rgb * lighting;
	
	
	//if (o_vertex_uv.y > u_progress)
        //discard; // Skip this fragment
	
	
    result_frag_color = vec4(lit_color, u_color.a * u_progress);
}
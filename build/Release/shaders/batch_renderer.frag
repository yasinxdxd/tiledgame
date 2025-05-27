#version 330 core

out vec4 result_frag_color;
in vec3 o_vertex_color;
in vec2 o_vertex_uv;


uniform vec4 u_color;
uniform float u_progress;

void main()
{
    float lighting = 0.8 + 0.5 * o_vertex_uv.y * o_vertex_uv.x;
	vec3 lit_color = u_color.rgb * lighting;
	
    // Combine vertex color with uniform color
    vec3 color = o_vertex_color * lit_color;

    // Apply animation effect using u_progress
    // Example: Fade opacity or scale color intensity
    float alpha = 1.0; // Default to opaque
    color *= (0.5 + 0.5 * u_progress); // Scale intensity (adjust as needed)
    // If u_progress controls opacity, uncomment: alpha = u_progress;

    // Fake beveled edge effect
    vec2 uv = o_vertex_uv;
    // Distance to nearest edge (rectangular, not radial)
    float edge_dist = min(min(uv.x, 1.0 - uv.x), min(uv.y, 1.0 - uv.y));
    float bevel_width = 0.09; // Controls bevel size (smaller = thinner bevel)
    float bevel = smoothstep(0.0, bevel_width, edge_dist);
    
    // Darken edges to mimic bevel (adjust factor for effect strength)
    color *= mix(0.7, 1.0, bevel); // Darken by 30% at edges, full color in center


    // -- post processing
    vec2 ofuv = clamp(uv, 0.1, 0.9);
    if (uv.x < ofuv.x || uv.x > ofuv.x || uv.y < ofuv.y || uv.y > ofuv.y) {
        color *= color * 0.7;
    }
    
    float shade_step = floor(ofuv.y * ofuv.x * 5.0); // 5 bands
    float shade = 0.6 + 0.2 * mod(shade_step, 5.0); // Vary from 0.2 to 1.0

    vec3 final_color = color * shade;


    // Output final color
    result_frag_color = vec4(final_color, alpha);
}
#version 430 core

out vec4 result_frag_color;
in vec3 o_vertex_color;
in vec2 o_vertex_uv;

uniform float time;

#define HASHSCALE1 443.8975


//
double zoom = pow(time/100, 2);
dvec2 center = vec2(0.3990264684, -0.129900895);
int itr = 200;
//

float hash13(vec3 p3)
{
	p3  = fract(p3 * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

// Noise functions by Dave Hoskins
//#define MOD3 vec3(.1031,.11369,.13787)
#define MOD3 vec3(.1731,.122689,.14687)
vec3 hash31(float p)
{
    vec3 p3 = fract(vec3(p) * MOD3);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract(vec3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}

vec4 map_to_color(float t) {
    //float r = 9.0 * (1.0 - t) * t * t * t;
    //float g = 15.0 * (1.0 - t) * (1.0 - t) * t * t;
    //float b = 8.5 * (1.0 - t) * (1.0 - t) * (1.0 - t) * t;

    return vec4(hash31(t), 1.0);//vec4(r, g, b, 1.0)
}

void main()
{
    dvec2 z, c;
    c.x = o_vertex_uv.x - 0.5;
    c.y = o_vertex_uv.y - 0.5;

    c.x /= zoom;
    c.y /= zoom;

    c.x += center.x;
    c.y += center.y;

    int i;
    for(i = 0; i < itr; i++) {
        double x = (z.x * z.x - z.y * z.y) + c.x;
		double y = (z.y * z.x + z.x * z.y) + c.y;

		if((x * x + y * y) > 4.4444) break;
		z.x = x;
		z.y = y;
    }

    double t = double(i) / double(itr);

    result_frag_color = map_to_color(float(t));
}

#version 430 core
layout (location = 0) in vec3 i_position; // the position variable has attribute position 0
layout (location = 1) in vec2 i_uv;
layout (location = 2) in vec3 i_color;


out vec3 o_vertex_color; // specify a color output to the fragment shader
out vec2 o_vertex_uv; // specify a uv output to the fragment shader

// uniform variables
// MVP matricies
uniform mat4 m_model;
uniform mat4 m_view;
uniform mat4 m_projection;

#define MAX_TOTAL_JOINT_COUNT 64
uniform mat4 m_joint_matrix[MAX_TOTAL_JOINT_COUNT];
uniform int m_joint_index;

vec4 calculate_2d_space(vec3 pos)
{
    vec4 result = m_projection * m_view * m_model * vec4(pos, 1.0) * mat4(1.0);//m_joint_matrix[0];
    return result;
}

vec4 weighted_joint_positions(vec3 pos)
{
    // first think that only one joint is related for a vertex.
    vec4 totalLocalPos = vec4(0.0);
    //0 - 244615
    //0 - 42
    int guessed_joint_index = int(abs(gl_VertexID / 5800.0));

    mat4 jointTransform = m_joint_matrix[guessed_joint_index];
    vec4 posePosition = jointTransform * vec4(pos, 1.0);
    totalLocalPos += posePosition * 1.0;

    return totalLocalPos;
}

void main()
{
    // gl_Position = calculate_2d_space(i_position);//vec4(i_position, 1.0);
    vec4 totalLocalPos = weighted_joint_positions(i_position);
    
    gl_Position = m_projection * m_view * m_model * vec4(i_position, 1.0);
    
    if (i_position.x > -3. && i_position.x < 3. && i_position.y > 9.8 && i_position.y < 10.1)
        gl_Position = m_projection * m_view * m_model * totalLocalPos;
    
    //send to fragment shader
    o_vertex_color = i_color;
    o_vertex_uv = i_uv;
}
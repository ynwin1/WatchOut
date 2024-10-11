#version 330

// Input attributes
in vec3 in_position;
in vec3 in_color; // Declare input for color

// Application data
uniform mat3 transform;
uniform mat3 projection;

// Passed to fragment shader
out vec3 frag_color; // Output color to fragment shader

void main()
{
    vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
    gl_Position = vec4(pos.xy, in_position.z, 1.0);
    frag_color = in_color; // Pass color to fragment shader
}

#version 330

// From vertex shader
in vec3 frag_color; // Receive the color

// Output color
layout(location = 0) out vec4 out_color;

void main()
{
    out_color = vec4(frag_color, 1.0); // Use the vertex color for output
}

#version 330

uniform vec3 entity_colour;

// Output color
layout(location = 0) out vec4 out_color;

void main()
{
	out_color = vec4(entity_colour, 1.0);
}

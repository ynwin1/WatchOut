#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec4 entity_colour;

// Output colour
layout(location = 0) out  vec4 colour;

void main()
{
	colour = entity_colour * texture(sampler0, vec2(texcoord.x, texcoord.y));
}

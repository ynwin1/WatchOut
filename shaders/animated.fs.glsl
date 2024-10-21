#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 entity_colour;

// Animation data
uniform float numFrames; 
uniform float currentFrame;

// Output colour
layout(location = 0) out  vec4 colour;

void main()
{
	colour = vec4(entity_colour, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
}

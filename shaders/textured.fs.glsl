#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 entity_colour;

// Lighting data
uniform float ambient_light;

// Output colour
layout(location = 0) out  vec4 colour;

void main()
{
	// Colour of raw texture/ damage effect 
	vec4 initialColour = vec4(entity_colour, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	// ambient light
	vec4 ambient = vec4(ambient_light * initialColour.rgb, initialColour.a);
	colour = ambient;
}

#version 330

uniform vec4 entity_colour;

// Output color
layout(location = 0) out vec4 out_color;

// Lighting data
uniform float ambient_light;

void main()
{
	vec4 ambient = vec4(ambient_light * entity_colour.rgb, entity_colour.a);
	out_color = ambient;
}

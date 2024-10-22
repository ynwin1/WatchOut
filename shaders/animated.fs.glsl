#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sprite_sheet;
uniform vec3 entity_colour;

// Animation data
 uniform float num_frames; 
 uniform float current_frame;

// Output colour
layout(location = 0) out  vec4 colour;

void main()
{
	float frame_width = 1.0 / num_frames;
	vec2 frame_texcoord = vec2(texcoord.x * frame_width + current_frame*frame_width, texcoord.y);
	colour = vec4(entity_colour, 1.0) * texture(sprite_sheet, vec2(frame_texcoord.x, frame_texcoord.y));
}

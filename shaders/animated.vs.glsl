#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat4 transform;
uniform mat4 projection;
uniform mat4 flatten;

void main()
{
	texcoord = in_texcoord;
	vec4 pos = projection * flatten * transform * vec4(vec3(in_position.xy, 1.0), 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}
#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;
uniform mat4 modelMatrix;

// Passed to fragment shader
out vec2 texcoord;
out vec3 worldPos;

// Application data
uniform mat3 transform;
uniform mat3 projection;

void main()
{
	texcoord = in_texcoord;
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);

	worldPos = (modelMatrix *  vec4(in_position.xy, 1.0, 1.0)).xyz;
}
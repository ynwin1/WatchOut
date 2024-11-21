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
uniform mat4 transform4;   
uniform mat4 projection4;
uniform int toScreen;

void main()
{
    vec4 pos;

    if (toScreen == 1) { // use mat4
        pos = projection4 * transform4 * vec4(in_position.xy, 0.0, 1.0);

		// flip the texture coordinates along the y-axis
		texcoord = vec2(in_texcoord.x, 1.0 - in_texcoord.y);
    } else {
        pos = projection * flatten * transform * vec4(vec3(in_position.xy, 1.0), 1.0); // convert to vec4
		texcoord = in_texcoord;
    }

    gl_Position = pos;
}
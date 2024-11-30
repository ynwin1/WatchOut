#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in mat3 transform;

uniform mat3 projection;

out vec2 texcoord;

void main()
{
    gl_Position = vec4(projection * transform * vec3(aPos.xy, 1.0), 1.0);
    texcoord = inTexCoord;
}  
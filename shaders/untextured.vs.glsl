#version 330

// !!! Simple shader for colouring basic meshes

// Input attributes
in vec3 in_position;

// Application data
uniform mat3 transform;  
uniform mat3 projection;   
uniform mat4 transform4;   
uniform mat4 projection4;  

uniform int toScreen;

void main()
{
    vec4 pos; 

    if (toScreen == 1) { // use mat4
        pos = projection4 * transform4 * vec4(in_position.xy, 0.0, 1.0);
    } else {
        pos = vec4(projection * transform * vec3(in_position.xy, 1.0), 1.0); // convert to vec4
    }

    gl_Position = pos;
}

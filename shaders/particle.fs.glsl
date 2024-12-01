#version 330 core
out vec4 FragColor;
  
in vec2 texcoord;
uniform sampler2D sampler0;
uniform float opacity;

void main()
{
    vec4 colour = texture(sampler0, texcoord);
    colour.a *= opacity;
    FragColor = colour;
}
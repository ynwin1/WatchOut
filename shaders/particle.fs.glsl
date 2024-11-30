#version 330 core
out vec4 FragColor;
  
in vec2 texcoord;
uniform sampler2D sampler0;

void main()
{
    FragColor = texture(sampler0, texcoord);
}
#version 330 core
out vec4 FragColor;

uniform vec3 u_lightColor;

void main()
{
    FragColor = vec4(u_lightColor, 1.0f); // setting values based on uniform
}
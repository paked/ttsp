#version 330 core

in vec4 Color;
out vec4 FragColor;

void main() {
    FragColor = Color;

    float gamma = 2.2;
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}

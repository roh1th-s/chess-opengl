#version 330 core

in vec2 texCoord;

uniform sampler2D tex;
uniform vec3 textColor;

out vec4 fragColor;

void main()
{ 
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(tex, texCoord).r);
    fragColor = vec4(textColor, 1.0) * sampled;
};
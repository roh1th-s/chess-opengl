#version 330 core

in vec2 texCoord;

uniform sampler2D tex;
uniform vec4 textColor;

out vec4 fragColor;

void main()
{ 
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(tex, texCoord).r);
    fragColor = textColor * sampled;
};
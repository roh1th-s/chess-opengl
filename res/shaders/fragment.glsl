#version 330 core

// fragment shaders run every fragment and determine color

layout(location = 0) out vec4 fragColor;

in vec3 vertexColor;
in vec2 texCoord;

uniform sampler2D wallTex;

void main()
{ 
   // fragColor = texture(wallTex, texCoord) * vec4(vertexColor, 1.0);
   fragColor = vec4(vertexColor, 1.0);
};
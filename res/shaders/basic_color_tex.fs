#version 330 core

in vec4 vertexColor;
in vec2 texCoord;

uniform sampler2D tex;
uniform int useTexture;

out vec4 fragColor;

void main()
{ 
   if (useTexture == 1)
      fragColor = texture(tex, texCoord) * vertexColor;
   else
      fragColor = vertexColor;
}
#version 330 core

in vec3 vertexColor;
in vec2 texCoord;

uniform sampler2D tex;
uniform int useTexture;

out vec4 fragColor;

void main()
{ 
   if (useTexture == 1)
      fragColor = texture(tex, texCoord) * vec4(vertexColor, 1.0);
   else
      fragColor = vec4(vertexColor, 1.0);
};
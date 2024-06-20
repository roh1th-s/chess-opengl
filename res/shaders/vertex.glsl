# version 330 core

// vertex shaders take vertexes from vertex buffer and determine where on screen to put it

// open gl will automatically take our 2 coordinates and make it at vec4
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoordIn;

out vec3 vertexColor;
out vec2 texCoord;

void main()
{
   gl_Position = position;
   vertexColor = color;
   texCoord = texCoordIn; 
};
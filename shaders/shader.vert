#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec3 aNorm;
layout (location = 3) in float aAmb;
  
out vec2 texCoord;
out vec3 normal;
out float ambient;

uniform mat4 MVP;

void main()
{
	// Transform vertices by MVP
    gl_Position = MVP * vec4(aPos, 1.0);

	// Forward variables to fragment shader
    texCoord = aTex;
	normal = aNorm;
	ambient = aAmb;
}
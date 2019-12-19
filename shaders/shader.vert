#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec3 aNorm;
layout (location = 3) in float aAmb;
  
out vec2 TexCoord;
out vec3 Normal;
out float Ambient;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(aPos, 1.0);
    TexCoord = aTex;
	Normal = aNorm;
	Ambient = aAmb;
}
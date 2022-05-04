layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec3 aNorm;
layout (location = 3) in float aAmb;
  
out vec2 texCoord;
out vec3 normal;
out float ambientV;
out vec3 worldPosition;

uniform mat4 modelMatrix;
uniform mat4 cameraMatrix;

void main()
{
	// Transform vertices by MVP
    vec4 world = modelMatrix * vec4(aPos, 1.0);
	worldPosition = world.xyz;
	gl_Position = cameraMatrix * world;

	// Forward variables to fragment shader
    texCoord = aTex;
	normal = aNorm;
	ambientV = aAmb;
}
layout (location = 0) in vec3 aPos;

uniform mat4 cameraMatrix;
uniform mat4 modelMatrix;

void main()
{
	// Transform vertices by MVP
    gl_Position = cameraMatrix * modelMatrix * vec4(aPos, 1.0);
}
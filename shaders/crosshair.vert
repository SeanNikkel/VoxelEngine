layout (location = 0) in vec3 aPos;

uniform vec2 size;

void main()
{
	// Just scale
	gl_Position = vec4(aPos.xy * size, aPos.z, 1.0);
}
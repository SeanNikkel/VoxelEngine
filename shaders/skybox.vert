layout (location = 0) in vec3 aPos;

out vec3 viewDir;

uniform mat4 invOriginCamMat;

void main()
{
	gl_Position = vec4(aPos, 1.0); // Render to screen space

	// View vector that is interpolated for fragment shader
	vec4 end = invOriginCamMat * vec4(aPos.xy, 1.0, 1.0);
	viewDir = normalize(end.xyz / end.w);
}
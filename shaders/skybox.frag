out vec4 fragColor;

in vec3 viewDir; // normalize(!)

void main()
{
	vec3 view = normalize(viewDir);

	fragColor = vec4(mix(vec3(FOG_COLOR), vec3(SKY_COLOR), max(view.y, 0.0)), 1.0);							// Sky color
	fragColor.xyz += pow(max(0.0, dot(view, normalize(vec3(LIGHT_DIR)))), 500.0) * vec3(SUN_COLOR);			// Sun
	fragColor.xyz += 0.25 * pow(max(0.0, dot(view, normalize(vec3(LIGHT_DIR)))), 2.0) * vec3(SUN_COLOR);	// Sun light
} 
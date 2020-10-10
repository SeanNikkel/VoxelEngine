#version 330 core

out vec4 fragColor;

in vec2 texCoord;
in vec3 normal;
in float ambient;

uniform sampler2D tex;
uniform float fogMin;
uniform float fogMax;
uniform vec3 fogColor;

void main()
{
	vec4 texCol = texture(tex, texCoord);

	// Directional diffuse
	float sun = max(0.5, dot(normal, vec3(-0.8, 1.0, -0.6)));

	// Ambient occlusion light value
	float ambient = (ambient + 1.0) / 4.0;

	// Fog amount
	float fog = clamp(((gl_FragCoord.z / gl_FragCoord.w) - fogMin) / (fogMax - fogMin), 0.0, 1.0);

	// Output fragment
    fragColor = vec4(texCol.rgb * sun * ambient, texCol.a);
	fragColor = mix(fragColor, vec4(fogColor, 1.0), fog); // apply fog
} 
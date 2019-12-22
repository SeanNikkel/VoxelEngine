#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in float Ambient;

uniform sampler2D tex;
uniform float fogMin;
uniform float fogMax;
uniform vec3 fogColor;

void main()
{
	vec4 texCol = texture(tex, TexCoord);
	float sun = max(0.5, dot(Normal, vec3(-0.8, 1.0, -0.6)));
	float ambient = (Ambient + 1.0) / 4.0;
	float fog = clamp(((gl_FragCoord.z / gl_FragCoord.w) - fogMin) / (fogMax - fogMin), 0.0, 1.0);
    FragColor = vec4(texCol.rgb * sun * ambient, texCol.a);
	FragColor = mix(FragColor, vec4(fogColor, 1.0), fog);
} 
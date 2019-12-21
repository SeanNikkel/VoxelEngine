#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in float Ambient;

uniform sampler2D tex;

void main()
{
	vec4 texCol = texture(tex, TexCoord);
	float sun = max(0.5, dot(Normal, vec3(-0.8, 1.0, -0.6)));
	float ambient = (Ambient + 1.0) / 4.0;
	float depth = clamp((800.0 - (gl_FragCoord.z / gl_FragCoord.w)) / 500.0, 0.0, 1.0);
    FragColor = vec4(texCol.rgb * sun * ambient, texCol.a);
	FragColor = mix(vec4(0.4, 0.7, 1.0, 1.0), FragColor, depth);
} 
out vec4 fragColor;

in vec2 texCoord;
in vec3 normal;
in float ambientV;
in vec3 worldPosition;

// Diffuse texture
uniform sampler2D tex;

// Camera info
uniform float nearPlane;
uniform float farPlane;
uniform float fogAmount;
uniform vec3 cameraPosition;

// Cascade info
uniform sampler2D cascades[MAX_CASCADES];
uniform float cascadeDepths[MAX_CASCADES];
uniform mat4 cascadeTransforms[MAX_CASCADES];

// Nonlinear [0, 1] depth to linear [0,1] depth
float ToLinear(float depth)
{
	return nearPlane * depth / (farPlane + depth * (nearPlane - farPlane));
}

// Linear [0, 1] depth to nonlinear [0,1] depth
float FromLinear(float depth)
{
	return farPlane * depth / (nearPlane - depth * (nearPlane - farPlane));
}

float Luminance(vec3 color)
{
	return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

// Calculate shadowing factor
float ShadowFactor()
{
	float depth = gl_FragCoord.z;
	float linearDepth = ToLinear(depth);

	// Determine cascade to use based on depth
	int cascade;
	for (int i = 0; i < MAX_CASCADES; i++)
	{
		if (linearDepth <= cascadeDepths[i])
		{
			cascade = i;
			break;
		}
	}

	// Calculate fragment position in light space of cascade given
	vec4 lightSpace = cascadeTransforms[cascade] * vec4(worldPosition, 1.0);
	lightSpace /= lightSpace.w;
	lightSpace = lightSpace * 0.5 + 0.5;

	// Shadow bias based on normal
	float bias = 0.005 * max((1.0 - dot(normal, vec3(LIGHT_DIR))), 0.1) / FromLinear(cascadeDepths[cascade]);

	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / vec2(textureSize(cascades[cascade], 0));
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			// Do shadow inequality for x and y
			float shadowDepth = texture(cascades[cascade], vec2(lightSpace.xy + vec2(x, y) * texelSize)).r; 
			shadow += lightSpace.z - bias < shadowDepth ? 1.0 : 0.0;
		}
	}

	return shadow / 9.0;
}

void main()
{
	vec3 N = normalize(normal);

	// Texture color
	vec3 texColor = texture(tex, texCoord).rgb;

	// View vector
	vec3 view = worldPosition - cameraPosition;
	float viewDist = length(view);
	view /= viewDist;

	// Directional diffuse
	float diffuse = max(0.0, dot(N, vec3(LIGHT_DIR)));

	// Shadow amount
	float shadowFactor = ShadowFactor();

	// Specular highlight
	float specular = 0.75 * pow(max(0.0, dot(reflect(view, N), vec3(LIGHT_DIR))), 1.0) * max(dot(N, vec3(LIGHT_DIR)), 0.0) * Luminance(texColor);

	// Ambient lighting
	vec3 ambient = vec3(FOG_COLOR) * texColor * 0.7;

	// Ambient occlusion light value
	float ambientOcclusion = (ambientV + 1.0) / 4.0;

	// Fog (from https://iquilezles.org/articles/fog/)
	float falloff = 0.02;
	float fogAmount = 0.5 * exp(-cameraPosition.y * falloff) * (1.0 - exp(-viewDist * view.y * falloff)) / view.y;
	fogAmount = clamp(fogAmount, 0.0, 1.0);
	vec3 fogColor = mix(vec3(FOG_COLOR), vec3(1.0, 0.9, 0.7), 0.5 * pow(max(0.0, dot(view, vec3(LIGHT_DIR))), 2.0));

	// Lighting calculation
    vec3 color = vec3(
		vec3(SUN_COLOR) * texColor * diffuse * shadowFactor +	// diffuse
		vec3(SUN_COLOR) * specular * shadowFactor +				// sepcular
		ambient * ambientOcclusion								//ambient
		);
	color = mix(color, fogColor, fogAmount); // apply fog

	// Output fragment
	fragColor = vec4(color, 1.0);
} 
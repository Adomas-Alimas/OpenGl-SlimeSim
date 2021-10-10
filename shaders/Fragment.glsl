#version 450 core
out vec4 FragColor;

layout (binding = 2) uniform sampler2D trailMap;
layout (binding = 3, rgba32f) uniform image2D inputTexture;

struct settingsStruct {
	float moveSpeed;
	float turnSpeed;
	float sensorAngle;
	float sensorDistance;

	int width;
	int height;

	float decayRate;
	float diffuseRate;
};

layout (std430, binding = 4) buffer settingsBuffer
{
	settingsStruct settings;
};

void main()
{
	int width = settings.width;
	int height = settings.height;

	float decayRate = settings.decayRate;
	float diffuseRate = settings.diffuseRate;
	// -------------------------------------

	ivec2 id = ivec2(gl_FragCoord.xy);

	// get original color for each fragment
	vec4 originalColor = texelFetch(trailMap, ivec2(gl_FragCoord.xy), 0).rgba;

	// simple box blur
	vec4 blurSum;
	int totalWeight;
	for(int offsetX = -1; offsetX <= 1; offsetX++)
	{
		for(int offsetY = -1; offsetY <= 1; offsetY++)
		{
			//blurSum += imageLoad(inputTexture, ivec2(tempX, tempY)).rgb;
			int sampleX = min(width-1, max(0, id.x+offsetX));
			int sampleY = min(height-1, max(0, id.y+offsetY));

			blurSum = blurSum + texelFetch(trailMap, ivec2(sampleX, sampleY), 0).rgba;
			totalWeight+= 1;
		}
	}

	blurSum /= totalWeight;

	// calculate diffusion + decay
	float diffuseWeight = clamp(diffuseRate, 0, 1);

	vec4 blurredColor = originalColor * (1 - diffuseWeight) + blurSum * diffuseWeight;
	

	imageStore(inputTexture, id, max(blurredColor - decayRate, 0.0f));  
	
	FragColor = originalColor;
}
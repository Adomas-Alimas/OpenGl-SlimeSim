#version 450 core
out vec4 FragColor;

layout (binding = 1, rgba32f) uniform image2D trailMap;
layout (binding = 2, rgba32f) uniform image2D agentMap;

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

layout (std430, binding = 3) buffer settingsBuffer
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


	// get original color for each pixel(fragment)
	vec4 originalColor = imageLoad(trailMap, ivec2(gl_FragCoord.xy)).rgba;
	

	// simple box blur
	vec4 blurSum;
	int totalWeight;
	for(int offsetX = -1; offsetX <= 1; offsetX++)
	{
		for(int offsetY = -1; offsetY <= 1; offsetY++)
		{
			
			int sampleX = min(width-1, max(0, int(gl_FragCoord.x)+offsetX));
			int sampleY = min(height-1, max(0, int(gl_FragCoord.y)+offsetY));

			// using imageLoad
			blurSum += imageLoad(trailMap, ivec2(sampleX, sampleY)).rgba;
			totalWeight+= 1;
		}
	}

	blurSum /= totalWeight;

	// calculate diffusion + decay
	float diffuseWeight = clamp(diffuseRate, 0, 1);

	vec4 blurredColor = originalColor * (1 - diffuseWeight) + blurSum * diffuseWeight;
	blurredColor = blurredColor - decayRate;
	blurredColor.a = 1;

	// store blurred + decayed trail in trailMap
	imageStore(trailMap, ivec2(gl_FragCoord.xy), max(blurredColor, 0.0f));  
	
	// load agent color from agentMap
	vec4 agentColor = imageLoad(agentMap, ivec2(gl_FragCoord.xy)).rgba;

	// if agent exists on this pixel show agent not trail
	if (agentColor.a > 0.1)
	{
		FragColor = agentColor;
	}
	else
	{	
		// else show trail not agent
		FragColor = originalColor;
	}

	// fixes left bottom corner white pixel bug?
	//imageStore(trailMap, ivec2(0, 0), vec4(0, 0, 0, 0));

	// clear the agent map, it will be filled by compute shader next 
	imageStore(agentMap, ivec2(gl_FragCoord.xy), vec4(0, 0, 0, 0));
}
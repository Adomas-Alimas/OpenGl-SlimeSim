#version 450 core
out vec4 FragColor;

// image textures used
layout (binding = 1, rgba32f) uniform image2D trailMap;
layout (binding = 2, rgba32f) uniform image2D agentMap;

// setting SSBO
struct settingsStruct {
	// agent settings
	// --------------
	float moveSpeed;
	float turnSpeed;
	float sensorAngle;
	float sensorDistance;

	// map size settings
	// ------------
	int width;
	int height;

	// diffusion and decay settings
	// ----------------------------
	float color_r;
	float color_g;
	float color_b;
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
	

	// box blur by sampling 3x3 area around the current fragment(pixel)
	// adding up all of the area color values and dividing them by 9
	// ----------------------------------------------------------------
	vec4 blurredColor = vec4(0);
	int totalWeight = 0;
	for(int offsetX = -1; offsetX <= 1; offsetX++)
	{
		for(int offsetY = -1; offsetY <= 1; offsetY++)
		{
			
			int sampleX = min(width-1, max(0, int(gl_FragCoord.x)+offsetX));
			int sampleY = min(height-1, max(0, int(gl_FragCoord.y)+offsetY));

			// using imageLoad
			blurredColor += imageLoad(trailMap, ivec2(sampleX, sampleY)).rgba;
			totalWeight+= 1;
		}
	}

	blurredColor /= totalWeight;

	float diffuseWeight = clamp(diffuseRate, 0, 1);


	// the new color (calculatedTrailColor) is composed out of originalColor 
	// and blurredColor, using diffuseWeight as the ratio
	vec4 calculatedTrailColor = originalColor * (1 - diffuseWeight) + blurredColor * diffuseWeight;

	// apply decay to color value
	calculatedTrailColor = calculatedTrailColor - decayRate;

	// fix alpha channel (has to always be 1)
	calculatedTrailColor.a = 1;
	

	// store blurred + decayed trail in trailMap
	imageStore(trailMap, ivec2(gl_FragCoord.xy), max(calculatedTrailColor, 0.0f));  
	
	// load agent color from agentMap
	vec4 agentColor = imageLoad(agentMap, ivec2(gl_FragCoord.xy)).rgba;

	// if agent exists (the pixel in agent map has alpha channel) 
	// agentColor not trail
	if (agentColor.a > 0.1)
	{
		FragColor = agentColor;
	}
	else
	{	
		// else show trail not agent
		FragColor = calculatedTrailColor;
	}

	// fixes left bottom corner white pixel bug?
	//imageStore(trailMap, ivec2(0, 0), vec4(0, 0, 0, 0));

	// clear the agent map, it will be filled by compute shader next iteration
	imageStore(agentMap, ivec2(gl_FragCoord.xy), vec4(0, 0, 0, 0));
}
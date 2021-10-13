#version 450 core
#define PI 3.1415926535
// local workgroup size
layout (local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

layout (location = 0) uniform float time;

layout (binding = 1, rgba32f) uniform image2D trailMap;
layout (binding = 2, rgba32f) uniform image2D agentMap;

// setting SSBO
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

// agents SSBO
struct agent {
	float x;
	float y;
	float angle;
};
layout (std430, binding = 4) buffer agentBuffer
{
	agent agentArray[];
};

uint hash(uint state)
{
	// returns pseudo-random result
	state ^= 2747636419u;
	state *= 2654435769u;
	state ^= state >> 16;
	state *= 2654435769u;
	state ^= state >> 16;
	state *= 2654435769u;
	return state;
}

float uintToRange01(uint state)
{
	// make a usigned int correspond to a value between 0 and 1
	float res = state / 4294967295f;
	return res;
}

float senseTrail(agent cAgent, float sensorAngleOffset, float sensorDistance)
{
	float sensorAngle = cAgent.angle + sensorAngleOffset;

	int sensorCenterX = int(cAgent.x + cos(sensorAngle) * sensorDistance);
	int sensorCenterY = int(cAgent.y + sin(sensorAngle) * sensorDistance);

	float senseSum;
	for (int offsetX = -1; offsetX <= 1; offsetX++)
	{
		for (int offsetY = -1; offsetY <= 1; offsetY++)
		{
			int sampleX = min(settings.width-1, max(0, sensorCenterX+offsetX));
			int sampleY = min(settings.height-1, max(0, sensorCenterY+offsetY));

			senseSum += dot(imageLoad(trailMap, ivec2(sampleX, sampleY)), vec4(1,1,1,1));
			//senseSum += imageLoad(trailMap, ivec2(sampleX, sampleY)).b;
		}
	}

	return senseSum;
}


void main()
{
	// get settings from settings SSBO

	int width = settings.width;
	int height = settings.height;

	float moveSpeed = settings.moveSpeed;
	float turnSpeed = settings.turnSpeed;

	float agentSensorAngleOffset = settings.sensorAngle;
	float sensorDistance = settings.sensorDistance;

	//-------------------------------------------

	ivec2 id = ivec2(gl_GlobalInvocationID.xy);
	
	// skip if compute shader invocation is too big
	if (id.x > agentArray.length())
	{
		return;
	}

	agent currentAgent = agentArray[id.x];
	
	// get a random number from a set of inputs
	uint random = hash(int(currentAgent.y * width + currentAgent.x) + hash(int(id.x + time * 100000)));

	float senseForward = senseTrail(currentAgent, 0, sensorDistance);
	float senseLeft = senseTrail(currentAgent, agentSensorAngleOffset, sensorDistance);
	float senseRight = senseTrail(currentAgent, -agentSensorAngleOffset, sensorDistance);
 
	float randomSteerStrength = uintToRange01(hash(random));

	// choose direction based on trails
	if(senseForward > senseLeft && senseForward > senseRight)
	{
		currentAgent.angle += 0;
	}
	else if (senseForward < senseLeft && senseForward < senseRight)
	{
		currentAgent.angle += (randomSteerStrength-0.5) * 2 * turnSpeed;
	}
	else if (senseLeft > senseRight)
	{
		currentAgent.angle += (randomSteerStrength * turnSpeed);
	}
	else if (senseLeft < senseRight)
	{
		currentAgent.angle -= (randomSteerStrength * turnSpeed);
	}
	
	// calculate next position
	currentAgent.x += moveSpeed * cos(currentAgent.angle);
	currentAgent.y += moveSpeed * sin(currentAgent.angle);
	
	// bound checking
	if (currentAgent.x <= 0 || currentAgent.x >= width || currentAgent.y <= 0 || currentAgent.y >= height)
	{
		uint random = hash(random);
		float randomAngle = uintToRange01(random) * 2 * PI;

		currentAgent.x = min(width-1, max(0, currentAgent.x));
		currentAgent.y = min(height-1, max(0, currentAgent.y));
		currentAgent.angle = randomAngle;
	}

	// store calculated agent int oagent array
	agentArray[id.x] = currentAgent;
	
	// agent color theme
	vec4 CONST_COLOR = vec4(1, 1, 1, 1);//(0.662, 0.282, 0.878, 1)

	// store aent color in agentMap
	imageStore(agentMap, ivec2(currentAgent.x, currentAgent.y), CONST_COLOR);
	
	// calculate and deposit trail into trailmap
	vec4 deposit = vec4(CONST_COLOR/5);
	deposit.a = 1;

	vec4 previousTrail = imageLoad(trailMap, ivec2(currentAgent.x, currentAgent.y));

	vec4 newTrail = vec4(min(previousTrail + deposit, CONST_COLOR));

	imageStore(trailMap, ivec2(currentAgent.x, currentAgent.y), newTrail);
}

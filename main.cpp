#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <Windows.h>
#define _USE_MATH_DEFINES // to get M_PI
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>

#include "json.hpp"
using json = nlohmann::json;


#include "shader.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);



int main()
{
	// get and read preset file into json object
	// --------------------------------------
	std::string settingsOption;
	std::cout << "Choose settings preset (found in presets folder): " << std::endl;
	std::cin >> settingsOption;
	
	std::string command = "presets/" + settingsOption + ".json";

	std::ifstream input(command);

	if (!input)
	{
		std::cout << "This preset file doesn't exist.";
		return -1;
	}

	json settingsData;

	

	input >> settingsData;

	// glfw setup

	unsigned int SCREEN_WIDTH = settingsData["mapWidth"];
	unsigned int SCREEN_HEIGHT = settingsData["mapHeight"];





	if (!glfwInit())
	{
		std::cout << "Failed to initialize glfw" << std::endl;
		return -1;
	}

	// setting OpenGL version, profiles, other settings
	// ------------------------------------------------
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	// remove after done
	//glfwWindowHint(GLFW_DECORATED, false);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Slime sim", NULL, NULL);
	//GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Slime sim", glfwGetPrimaryMonitor(), NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	
	// build and compile shader programs
	// --------------------------------
	vertFragShader generalShader("shaders/Vertex.glsl", "shaders/Fragment.glsl");

	// default to final compute shader
	computeShader simShader("shaders/slimeFinal.glsl");

	// choose simulation level based on settings preset
	if(settingsData["simulationShader"] == "stageFinal")
	{
		simShader = computeShader("shaders/slimeFinal.glsl");
	}
	else
	{
		std::string option = settingsData["simulationShader"];
		std::string shaderpath = "shaders/" + option + ".glsl";
		simShader = computeShader(shaderpath.c_str());
	}
	

	// set up vertex data and buffers
	// ------------------------------
	float rectangleVertices[] = {
		// rectangle is made from 2 triangles
		// positions        // colors          // texture coords
		1.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // top right
		1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // bot right
	   -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, // bot left
	   -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f // top left
	};

	unsigned int indices[] = { // order for drawing vertices
		0, 1, 3, // first trinagle
		1, 2, 3  // second triangle
	};


	// general VBO, VAO, EBO setup using data above
	// --------------------------------------------
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// bind vertex array object first, then bind and set vertex buffer, then configure vertex attributes
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// configure vertex attributes
	// ---------------------------
	// position attrib
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),  (void*)0);
	glEnableVertexAttribArray(0);

	// color attrib
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),  (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coord attrib
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),  (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// generate empty trail and agents textures and all params for it
	// --------------------------------------------------------------
	unsigned int trailTexture, agentTexture;
	glGenTextures(1, &trailTexture);
	glGenTextures(1, &agentTexture);

	// trail texture setup
	glBindTexture(GL_TEXTURE_2D, trailTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	// agent texture setup
	glBindTexture(GL_TEXTURE_2D, agentTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	float alphaVal[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	glClearTexImage(agentTexture, 0, GL_RGBA, GL_FLOAT, alphaVal);

	// setting up agent settings for compute shaders
	// ----------------------------------------------

	// create and populate settings struct
	struct settings{
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
		float decayRate;
		float diffuseRate;

	} generalSettings;

	// can't assign values to variables above from the json
	// file so i have to do that bellow
	generalSettings.moveSpeed = settingsData["moveSpeed"];
	generalSettings.turnSpeed = settingsData["turnSpeed"];
	generalSettings.sensorAngle = settingsData["sensorAngle"];
	generalSettings.sensorDistance = settingsData["sensorDistance"];

	generalSettings.width = settingsData["mapWidth"];
	generalSettings.height = settingsData["mapHeight"];

	generalSettings.decayRate = settingsData["decayRate"];
	generalSettings.diffuseRate = settingsData["diffuseRate"];

	// create settings SSBO and put settings struct into it
	unsigned int settingsSSBO;
	glGenBuffers(1, &settingsSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, settingsSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(generalSettings), &generalSettings, GL_STATIC_DRAW);

	
	// create agent struct and fill an array with agents
	// -------------------------------------------------
	unsigned int AGENT_NUM = settingsData["agentNumber"];

	struct agent {
		float x;
		float y;
		float angle; // radians
	};
	
	
	// !!danger zone, be careful with malloc and free it at the end
	// this is needed for bigger amount of agents that exceeds the max size
	// of default arrays in c++

	agent *agentsArrPtr;
	agentsArrPtr = (agent*) malloc(AGENT_NUM * sizeof(agent));

	// setup random device for angle, position, etc.. customization
	std::random_device rd;
	std::mt19937 gen(rd());
	

	// initialize each agent with starting pos, angle
	for (int i = 0; i < AGENT_NUM; i++)
	{
		// temp agent that gets added to agentsArrPtr
		agent t;

		// centre position x and y
		int centreX = SCREEN_WIDTH / 2;
		int centreY = SCREEN_HEIGHT / 2;


		if (settingsData["spawnMethod"] == "centre")
		{
			// spawns all agents in the middle, with random angles
			std::uniform_real_distribution<> randomAngle(0, 12.5662);
			t.x = centreX;
			t.y = centreY;
			t.angle = randomAngle(gen);
		}
		else if (settingsData["spawnMethod"] == "circle")
		{
			// spawns all agents in the area of a circle with angles
			// facing towards screen centre
			int radius = SCREEN_HEIGHT / 3;
			std::uniform_real_distribution<> randomAngle(0, 6.2831);
			std::uniform_int_distribution<> randomRadius(0, radius);

			int distance = randomRadius(gen);
			float genAngle = randomAngle(gen);

			t.x = centreX + (cos(genAngle) * distance);
			t.y = centreY + (sin(genAngle) * distance);

			t.angle = genAngle + M_PI;
		}
		else if (settingsData["spawnMethod"] == "random")
		{
			// spawns all agents with random angles and random position

			std::uniform_real_distribution<> randomAngle(0, 6.2831);
			std::uniform_int_distribution<> randomX(0, SCREEN_WIDTH);
			std::uniform_int_distribution<> randomY(0, SCREEN_HEIGHT);

			t.x = randomX(gen);
			t.y = randomY(gen);

			t.angle = randomAngle(gen);
		}

		agentsArrPtr[i] = t;
	}

	// create and fill SSBO with agent array created above
	// ---------------------------------------------------
	unsigned int agentDataSSBO;
	glGenBuffers(1, &agentDataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, agentDataSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, AGENT_NUM * sizeof(agent), agentsArrPtr, GL_DYNAMIC_READ);
	// ooga booga free memory to make pc no crash
	free(agentsArrPtr);

	// unbind VAO, saving all buffers into it, then unbind buffers, texture
	// --------------------------------------------------------------------
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);


	// wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	
	int flag = 0;
	// main loop

	while(!glfwWindowShouldClose(window))
	{
		// TODO: remove for debbuging
		if (flag == 0)
		{
			flag = 1;

		}
		else if (flag == 1)
		{
			system("pause");
			flag = 2;
		}

		// input
		// -----
		processInput(window);

		// clear screen
		// ------------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		// run general vertex and fragment shaders
		// ---------------------------------------
		generalShader.use();
		glBindVertexArray(VAO);

		// bind textures texture to bindings in frag shader
		glBindImageTexture(1, trailTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(2, agentTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

		// bind settings SSBO to binding = 3 in frag shader
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, settingsSSBO);
		
		// draw the mainTexture on a whole screen rectangle 
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// calculate new simulation step in compute shader
		// ---------------------------------
		simShader.use();

		float timeValue = glfwGetTime();
		simShader.setFloat("time", timeValue);

		// bind textures to bindings in compute shader
		glBindImageTexture(1, trailTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(2, agentTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

		// bind settings SSBO to binding = 3 in compute shader
		// bind agent array SSBO to binding = 4 in compute shader
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, settingsSSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, agentDataSSBO);

		// change this value to make compute shader more efficient (1, 8, 16, 32)
		const int computeDivisor = 32;

		simShader.dispatch(AGENT_NUM/computeDivisor, 1);

		// stops execution until all compute shaders have finished work
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		// glfw - swap buffers and poll events
		// -----------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}  

void processInput(GLFWwindow *window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
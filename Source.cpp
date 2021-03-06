#pragma once

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "GL.h"
#include "Geometry.h"
#include "Model.h"
#include "Shader.h"


/* TEMP GLOBALS */
Cube a = Cube(1.0f);

// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);

// Timing
auto t_start = std::chrono::high_resolution_clock::now();
auto t_end = std::chrono::high_resolution_clock::now();

// Input Handling
std::vector<bool> buttons(6);

// Camera matrix
float cameraSpeed = 20.0f;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -20.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 View = glm::lookAt(
	cameraPos, // Camera is at (4,3,3), in World Space
	cameraFront + cameraPos, // and looks at the origin
	cameraUp  // Head is up (set to 0,-1,0 to look upside-down)
);

// Model matrix : an identity matrix (model will be at the origin)
glm::mat4 Model = glm::mat4(1.0f);
// Our ModelViewProjection : multiplication of our 3 matrices
glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around


int 
main() {
	/* GLFW STUFF */
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr); // Windowed
	//GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, fallbackCameraKeyCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, fallbackMouseCallback);
	glfwSwapInterval(1);

	/* GLEW STUFF */
	glewExperimental = GL_TRUE;
	glewInit();

	/* GL SETTINGS */
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);

	/* Initialize Program */
	Shader program = Shader("shaders/default.vert", "shaders/default.frag");
	program.use();

	/* Initialize Cubes */
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo); // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* a.pVertices.size(), a.pVertices.data(), GL_STATIC_DRAW);

	GLint posAttrib = glGetAttribLocation(program.ID, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(float), 0);

	GLint normAttrib = glGetAttribLocation(program.ID, "normal");
	glEnableVertexAttribArray(normAttrib);
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(float), (void*)(3 * sizeof(float)));

	GLuint vboo;
	glGenBuffers(1, &vboo); // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, vboo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * a.offsetVertices.size(), a.offsetVertices.data(), GL_STATIC_DRAW);

	GLint offAttrib = glGetAttribLocation(program.ID, "offset");
	glEnableVertexAttribArray(offAttrib);
	glVertexAttribPointer(offAttrib, 3, GL_FLOAT, GL_FALSE,
		3 * sizeof(float), 0);
	glVertexAttribDivisor(offAttrib, 1);

	GLuint vboc;
	glGenBuffers(1, &vboc); // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, vboc);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * a.colorVertices.size(), a.colorVertices.data(), GL_STATIC_DRAW);
	GLint colAttrib = glGetAttribLocation(program.ID, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
		3 * sizeof(float), 0);
	glVertexAttribDivisor(colAttrib, 1);

	//GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
	//glEnableVertexAttribArray(texAttrib);
	//glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
	//	8 * sizeof(float), (void*)(6 * sizeof(float)));
	//glVertexAttribDivisor(texAttrib, 2);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* a.elements.size(), a.elements.data(), GL_STATIC_DRAW);

	// Texture
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
	int width, height;
	unsigned char* image = SOIL_load_image("images/cat.jpg", &width, &height, 0, SOIL_LOAD_RGB); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);

	// Get a handle for our "MVP" uniform
	// Only during the initialisation
	GLuint MatrixID = glGetUniformLocation(program.ID, "MVP");

	/* Initialize DK */
	Engine::Model DK = Engine::Model("models/dk/donkey kong.obj");
	Transformation dkt = { cameraPos + cameraFront, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.1) };
	DK.CreateInstance(dkt);
	Engine::Model Banana = Engine::Model("models/Golden Bananas/goldbananas.obj");
	Banana.CreateInstance({ glm::vec3(0.0,0.0,0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.1) });
	Banana.CreateInstance({ glm::vec3(0.0,0.0,0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.1) });
	Engine::Model Scene = Engine::Model("models/Whack-a-Plant/frame_29.08.2014_13.12.07.obj");
	Scene.CreateInstance({ glm::vec3(0.0,0.0,0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0) });
	while (!glfwWindowShouldClose(window))
	{
		/* POLL EVENTS */
		glfwPollEvents();
		// Escape
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);

		/* GAME ENGINE */
		t_end = std::chrono::high_resolution_clock::now();
		float delta = std::chrono::duration_cast<std::chrono::duration<float>>(t_end - t_start).count();
		t_start = std::chrono::high_resolution_clock::now();
		update(delta);
		dkt.Position = cameraPos + cameraFront * 5.0f;
		DK.UpdateInstance(dkt, 0);
		mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

		/* RENDER */
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Clear the screen to black
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		//glDrawElementsInstanced(GL_TRIANGLES, a.elements.size(), GL_UNSIGNED_INT, 0, 4);
		DK.Draw(program);
		Banana.Draw(program);
		Scene.Draw(program);

		/* SWAP */
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}


void
update(float delta)
{
	if (buttons[4]) { delta *= 2; }

	if (buttons[0]) { cameraPos += delta * cameraSpeed * cameraFront; }
	if (buttons[1]) { cameraPos -= delta * cameraSpeed * cameraFront; }
	if (buttons[2]) { cameraPos -= delta * glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed; }
	if (buttons[3]) { cameraPos += delta * glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed; }
	if (buttons[5]) {}
	
	// Update View
	View = glm::lookAt(
		cameraPos, // Camera is at (4,3,3), in World Space
		cameraFront + cameraPos, // and looks at the origin
		cameraUp  // Head is up (set to 0,-1,0 to look upside-down)
	);
}


void
fallbackCameraKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	buttons[0] = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
	buttons[1] = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
	buttons[2] = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
	buttons[3] = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
	buttons[4] = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
	buttons[5] = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
}


float lastX = 400, lastY = 300;
float yaw = 90; float pitch = 0;
float firstMouse = true;
void fallbackMouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

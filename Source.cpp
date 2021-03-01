#pragma once

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "GL.h"

/* TEMP GLOBALS */
float* vertices = createCube(1.0f);

GLuint elements[] = {
	0, 1, 2,
	2, 3, 0,
	4, 5, 6,
	6, 7, 4
};

// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);

// Timing
auto t_start = std::chrono::high_resolution_clock::now();
auto t_end = std::chrono::high_resolution_clock::now();

// Input Handling
std::vector<bool> buttons(6);

// Camera matrix
float cameraSpeed = 2.0f;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -2.0f);
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

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo); // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*64, vertices, GL_STATIC_DRAW);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

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

	GLuint shaderProgram = LoadShader("shaders/default.vert", "shaders/default.frag");
	glUseProgram(shaderProgram);

	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
		8 * sizeof(float), 0);

	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
		8 * sizeof(float), (void*)(3 * sizeof(float)));

	GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
		8 * sizeof(float), (void*)(6 * sizeof(float)));

	// Get a handle for our "MVP" uniform
	// Only during the initialisation
	GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");

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
		mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

		/* RENDER */
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Clear the screen to black
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

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

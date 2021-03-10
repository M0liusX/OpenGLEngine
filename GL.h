#pragma once

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL2/SOIL2.h>
#include <glm/gtc/matrix_transform.hpp>


void
fallbackCameraKeyCallback(GLFWwindow* window,
	                      int key,
	                      int scancode,
	                      int action,
	                      int mods);
void
fallbackMouseCallback(GLFWwindow* window,
	                  double xpos,
	                  double ypos);
void update(float delta);

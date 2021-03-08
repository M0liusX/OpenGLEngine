#pragma once

#include <vector>
#include "GL.h"

class Cube {
public:
	std::vector<float> pVertices; // position vertices
	std::vector<float> offsetVertices; // offset
	std::vector<float> colorVertices; // color
	std::vector<GLuint> elements; // vertice indices
	Cube(float size);
};
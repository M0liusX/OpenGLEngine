#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "Shader.h"


struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Transformation {
    glm::vec3 Position;
    glm::vec3 Rotation;
    glm::vec3 Scale;
};

struct Texture {
	unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    // mesh data
    std::vector<Vertex>            vertices;
    std::vector<unsigned int>      indices;
    std::vector<Texture>           textures;
    std::vector<Transformation>    instances;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(Shader& shader);
    void CreateInstance(Transformation const &transform);
    void UpdateInstance(Transformation const& transform, unsigned int instance);
private:
    //  render data
    unsigned int VAO, VBO, EBO, IBO;

    void setupMesh();
};


#endif

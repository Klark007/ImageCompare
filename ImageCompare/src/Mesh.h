#pragma once

#include "Program.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <memory>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texture_coord;
};

typedef unsigned int index;

class Mesh
{
public:
	Mesh() {};
	Mesh(std::vector<Vertex> vertices, std::vector<index> indices);
	void draw();

private:
	std::vector<Vertex> vertices;
	std::vector<index> indices;

	unsigned int vao, vbo, ebo;

	void setup_mesh();
};
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#include <vector>
#include <memory>

#define LOG_LENGTH 512

class Program
{
public:
	Program();
	Program(std::vector<std::shared_ptr<Shader>>& shaders);
	Program(const Program&) = default;
	~Program();

	void attach_shader(unsigned int id);
	void attach_shader(Shader& shader);
	void link_program();

	void use();
private:
	unsigned int program;
	char log[LOG_LENGTH];

	inline int get_location(const char* name) { return glGetUniformLocation(program, name); }
public:
	void get(unsigned int name, int* params);
	void print_link_error();
	inline unsigned int get_id() { return program; };

	void set1i(const char* name, int value);
	void set1f(const char* name, float value);

	void set_vec2f(const char* name, glm::vec2& value);
	void set_vec2f(const char* name, float x, float y);

	void set_vec3f(const char* name, glm::vec3& value);
	void set_vec3f(const char* name, float r, float g, float b);

	void set_mat4f(const char* name, glm::mat4& value);
};
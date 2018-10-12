#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "mesh.h"
#include "vertex.h"

std::vector<std::string> split(const char *str, char c = ' ')
{
	std::vector<std::string> result;
	do {
		const char *begin = str;
		while (*str != c && *str)
			str++;
		result.push_back(std::string(begin, str));
	} while (0 != *str++);
	return result;
}

void ParseAndAddVert(std::vector<Vertex>& verts, std::string& string)
{
	static char buf[64];
	float x, y, z;
	strcpy(buf, string.c_str());
	const char* str = buf + 1;
	while (*str == ' ') str++;
	x = (float)std::atof(str);
	while (*str != ' ') str++;
	while (*str == ' ') str++;
	y = (float)std::atof(str);
	while (*str != ' ') str++;
	while (*str == ' ') str++;
	z = (float)std::atof(str);

	Vertex vert;
	vert.position = glm::vec3(x, y, z);
	verts.push_back(vert);

	//std::cout << "V {" << x << ", " << y << ", " << z << "}\n" << string << "\n";
}

void ParseAndAddFace(std::vector<unsigned>& indices, std::string& string)
{
	static char buf[64];
	unsigned p, q, r;
	strcpy(buf, string.c_str());
	const char* str = buf + 1;
	while (*str == ' ') str++;
	p = (unsigned)std::atoi(str);
	while (*str != ' ') str++;
	while (*str == ' ') str++;
	q = (unsigned)std::atoi(str);
	while (*str != ' ') str++;
	while (*str == ' ') str++;
	r = (unsigned)std::atoi(str);
	
	indices.push_back(p);
	indices.push_back(q);
	indices.push_back(r);

	//std::cout << "F {" << p << ", " << q << ", " << r << "}\n" << string << "\n";
}

void Mesh::create_from_obj(std::string filename)
{
	

	//std::stringstream strstr;
	//strstr << ASSET_PATH << "models/" << objFile;
	std::ifstream input = std::ifstream(filename);
	assert(input.good());//"Cannot load mesh: assets/models/%s", objFile.c_str());

	unsigned length;


	input.seekg(0, std::ios::end);
	length = (unsigned)input.tellg();
	input.seekg(0, std::ios::beg);
	char* buffer = new char[length + 1];
	buffer[length] = 0;
	input.read(buffer, length);
	input.close();

	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;
	vertices.reserve(length);
	indices.reserve(length);

	std::vector<std::string> lines = split(buffer, '\n');
	for (auto& line : lines)
	{
		if (line.length() > 0 && line.at(0) == 'v'  && line.at(1) == ' ')
			ParseAndAddVert(vertices, line);
		else if (line.length() > 0 && line.at(0) == 'f' && line.at(1) == ' ')
			ParseAndAddFace(indices, line);
	}
	delete buffer;

	vertices.shrink_to_fit();
	indices.shrink_to_fit();
	
	//Preprocess();
}
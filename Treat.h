#pragma once
#include "glm/glm.hpp"

class Snake;

class Treat
{

public:
	Treat();

	void Draw();
	
	void Eaten(Snake* s);

	int getRand();
	
public:
	glm::vec3 pos;

};
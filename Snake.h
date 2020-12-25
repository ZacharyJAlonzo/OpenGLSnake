#pragma once
#include "glm/glm.hpp"
#include <vector>

class Treat;

enum Direction
{
	Up, 
	Down,
	Left,
	Right,
	NONE
};

struct PosDir
{
	glm::vec3 pos;
	Direction dir;
};

class Snake
{


public:
	
	Snake();
	
	void Draw(glm::mat4 view, glm::mat4 proj);
	
	//update direction if changed, update next locations to draw
	void UpdateSnake();

	void AddBody();
	
	std::vector<PosDir> snake;
	
	Direction CurrentDirection = Up;
	//updated on key press
	Direction NextDirection = NONE;

	Treat* treat;

private:
	

	unsigned int program;
	unsigned int mvpLoc;

	unsigned int VBO;
	unsigned int EBO;
	unsigned int VAO;

};

#include "Treat.h"
#include "Snake.h"
#include "GL/glew.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>


Treat::Treat()
{
	pos = glm::vec3(0.5f, 0.5f, 0.5f);
}

void Treat::Draw()
{
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);
}

void Treat::Eaten(Snake* s)
{
	int r = getRand();
	std::cout << r << std::endl;

	glm::vec3 newPos = glm::vec3(r + 0.5, 0.5, r + 0.5);
	bool done = false;
	while (!done)
	{
		for (int i = 0; i < s->snake.size(); i++)
		{
			if (newPos == s->snake.at(i).pos || newPos == pos)
			{
				i = 0;
				r = getRand();
				newPos = glm::vec3(r + 0.5, 0.5, r + 0.5);
			}
		}

		pos = newPos;
		done = true;
	}
}

int Treat::getRand()
{
	srand(time(NULL));
	return rand() % 10 - 5;
}

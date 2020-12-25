#pragma once

#include "Snake.h"
#include "GL/glew.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Treat.h"
#include <list>

Snake::Snake()
{
	//bottom left corner 
	snake = { {glm::vec3(-4.5, 0.5, 1.5), Up} };

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	float cube[] =
	{
		-0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,

		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f,
	};

	unsigned int cubeEbo[] =
	{
		0,1,
		1,2,
		2,3,
		3,0,
		0,4,
		1,5,
		2,6,
		3,7,
		4,5,
		5,6,
		6,7,
		7,4
	};

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeEbo), cubeEbo, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);


	program = glCreateProgram();

	const char* vsource =
		R"(
#version 330 core

layout(location = 0) in vec3 position;
uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(position, 1.0);
};
)";

	const char* fsource =
		R"(
#version 330 core

out vec4 color;
void main()
{
	color = vec4(0.f, 1.0f, 0.f, 1.0f);
};
)";

	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vs, 1, &vsource, nullptr);
	glShaderSource(fs, 1, &fsource, nullptr);

	glCompileShader(vs);
	glCompileShader(fs);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);
}

void Snake::Draw(glm::mat4 view, glm::mat4 proj)
{
	glUseProgram(program);
	glBindVertexArray(VAO);

	mvpLoc = glGetUniformLocation(program, "mvp");

	for (unsigned int i = 0; i < snake.size(); i++)
	{
		glm::mat4 MVP = glm::mat4(1.0);
		glm::mat4 model = glm::mat4(1.0);
		model = glm::translate(model, snake.at(i).pos);

		MVP = proj * view * model;
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

		glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);
	}

}

void Snake::UpdateSnake()
{
	std::list<glm::vec3> positions = {};
	for (unsigned int i = 0; i < snake.size(); i++)
	{
		if (i == 0)
		{
			snake.at(i).dir = CurrentDirection;
			if (snake.at(i).pos == treat->pos)
			{
				//add body
				treat->Eaten(this);
				AddBody();
			}
		}

		switch (snake.at(i).dir)
		{

		case Up:
			//-z is forwards
			snake.at(i).pos += glm::vec3(0, 0, -1.f);
			if (snake.at(i).pos.z < -4.5f)
				snake.at(i).pos.z = 4.5f;

			positions.push_back(snake.at(i).pos);
				

			break;
		case Down:
			snake.at(i).pos += glm::vec3(0, 0, 1.f);
			if (snake.at(i).pos.z > 4.5f)
				snake.at(i).pos.z = -4.5f;

			positions.push_back(snake.at(i).pos);

			break;
		case Right:
			snake.at(i).pos += glm::vec3(1.f, 0, 0);
			if (snake.at(i).pos.x > 4.5f)
				snake.at(i).pos.x = -4.5f;

			positions.push_back(snake.at(i).pos);

			break;
		case Left:
			snake.at(i).pos += glm::vec3(-1.f, 0, 0);
			if (snake.at(i).pos.x < -4.5f)
				snake.at(i).pos.x = 4.5f;

			positions.push_back(snake.at(i).pos);

		}

	}

	for (int i = snake.size() - 1; i > 0; i--)
	{
		if (i != 0)
		{
			if (snake.at(0).pos == snake.at(i).pos && i != 0)
			{
				exit(0);
			}
			snake.at(i).dir = snake.at(i - 1).dir;
		}
	}
}

void Snake::AddBody()
{
	switch (snake.at(snake.size() - 1).dir)
	{
	case Up:
		snake.push_back({  snake.at(snake.size()-1).pos, NONE });
		if (snake.at(snake.size() - 1).pos.z < -4.5f)
			snake.at(snake.size() - 1).pos.z = 4.5f;


		break;
	case Down:
		snake.push_back({ snake.at(snake.size() - 1).pos, NONE });
		if (snake.at(snake.size() - 1).pos.z > 4.5f)
			snake.at(snake.size() - 1).pos.z = -4.5f;

		break;
	case Right:
		snake.push_back({ snake.at(snake.size() - 1).pos, NONE });
		if (snake.at(snake.size() - 1).pos.x > 4.5f)
			snake.at(snake.size() - 1).pos.x = -4.5f;

		break;
	case Left:
		snake.push_back({ snake.at(snake.size() - 1).pos, NONE });
		if (snake.at(snake.size() - 1).pos.x < -4.5f)
			snake.at(snake.size() - 1).pos.x = 4.5f;
	
	}

}

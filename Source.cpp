#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<glm/glm.hpp>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int shaderProgram;

const float paddleSpeed = 0.001f;

unsigned int LScore = 0, RScore = 0;

class Paddle
{
	unsigned int VAO,VBO;

	void SetBufferData()
	{
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
		//std::cout << "BufferDataSet";
	}
public:
	glm::vec2 position;
	glm::vec3 vertices[6];
	float xWidth, yWidth;

	Paddle()
	{
		Paddle(0.0f, 0.0f);
	}
	Paddle(float x, float y)
	{
		xWidth = 0.025f;
		yWidth = 0.2f;

		position.x = x;
		position.y = y;

		vertices[0] = glm::vec3(xWidth, yWidth, 0.0f);//TR
		vertices[1] = glm::vec3(-xWidth, yWidth, 0.0f);//TL
		vertices[2] = glm::vec3(xWidth, -yWidth, 0.0f);//BR

		vertices[3] = glm::vec3(-xWidth, yWidth, 0.0f);//TL
		vertices[4] = glm::vec3(xWidth, -yWidth, 0.0f);//BR
		vertices[5] = glm::vec3(-xWidth, -yWidth, 0.0f);//BL

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		SetBufferData();
	}

	~Paddle()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}

	void draw()
	{
		glBindVertexArray(VAO);

		glUniform1f(glGetUniformLocation(shaderProgram, "xOffset"), position.x);

		glUniform1f(glGetUniformLocation(shaderProgram, "yOffset"), position.y);

		glUseProgram(shaderProgram);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		ClampY();


		//glBindVertexArray(0);
	}

	void ClampY()
	{
		if (position.y + yWidth > 1.0f)
			position.y = 1.0f - yWidth;

		if (position.y - yWidth < -1.0f)
			position.y = -1.0f + yWidth;
	}
	
}*RP,*LP;

class Ball
{
	unsigned int VAO, VBO;

	void SetBufferData()
	{
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER,VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);


		glBindVertexArray(0);
		//std::cout << "BufferDataSet";
	}
public:
	float radius;
	glm::vec2 position;
	glm::vec2 vel;
	glm::vec3 vertices[12];
	Ball()
	{
		Ball(0.0f, 0.0f, 0.05f);
	}
	Ball(float x,float y,float rad)
	{
		position.x = x;
		position.y = y;
		radius = rad;

		SetRandVelocity();

		for (int i = 0; i < 12; i++)
		{
			vertices[i] = glm::vec3(cos(glm::radians(i*30.0f)),sin(glm::radians(i*30.0f)) * ((float)SCR_WIDTH/SCR_HEIGHT),0)*radius;
		}

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		SetBufferData();
	}

	void draw()
	{
		glBindVertexArray(VAO);

		glUniform1f(glGetUniformLocation(shaderProgram, "xOffset"), position.x);

		glUniform1f(glGetUniformLocation(shaderProgram, "yOffset"), position.y);

		glUseProgram(shaderProgram);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 12);


		//glBindVertexArray(0);
		ClampY();

		position.x += vel.x;
		position.y += vel.y;

		CheckCollisionWithSideWalls();
	}

	void SetRandVelocity()
	{
		int randX, randY;

		do
		{
			randX = rand() % 10 - 5;
			randY = rand() % 10 - 5;
		} while (randX == 0 || randY == 0);

		vel = (glm::vec2(randX, randY)) * 0.00003f;

		
	}

	void ClampY()
	{
		if (position.y + radius > 1.0f)
			vel.y = -vel.y;
		if (position.y - radius < -1.0f)
			vel.y = -vel.y;
	}

	void ResetBall()
	{
		position = glm::vec2(0.0f, 0.0f);
	}

	void CheckCollisionWithSideWalls()
	{
		if (position.x + radius > 1.0f)
		{
			ResetBall();
			SetRandVelocity();

			LScore++;

			//std::cout << "\nPlayer 1 : " << LScore << " -  Player 2 : " << RScore;
			std::cout << LScore << "\t\t" << RScore << "\n";
		}
		if (position.x - radius < -1.0f)
		{
			ResetBall();
			SetRandVelocity();

			RScore++;

			//std::cout << "\nPlayer 1 : " << LScore << " -  Player 2 : " << RScore;
			std::cout << LScore << "\t\t" << RScore << "\n";
		}
	}

	void CheckCollisionWithLeftPaddle(const Paddle lp)
	{
		if (position.x - radius < lp.position.x + lp.xWidth )
		{
			if (position.y + radius < lp.position.y + lp.yWidth && position.y - radius > lp.position.y - lp.yWidth)
			{
				vel.x *= -1;
			}
		}
	}

	void CheckColWithLPaddle(glm::vec2 PaddlePosition, float xWidth, float yWidth)
	{
		if (position.x - radius < PaddlePosition.x + xWidth)
		{
			if (position.y + radius < PaddlePosition.y + yWidth && position.y - radius > PaddlePosition.y - yWidth)
			{
				vel.x = -vel.x;
			}
		}
	}

	void CheckColWithRPaddle(glm::vec2 PaddlePosition, float xWidth, float yWidth)
	{
		if (position.x + radius > PaddlePosition.x - xWidth)
		{
			if (position.y + radius < PaddlePosition.y + yWidth && position.y - radius > PaddlePosition.y - yWidth)
			{
				vel.x = -vel.x;
			}
		}
	}
	void CheckCollisionWithRightPaddle(const Paddle rp)
	{
		if (position.x + radius > rp.position.x - rp.xWidth)
		{
			if (position.y + radius < rp.position.y + rp.yWidth && position.y + radius > rp.position.y - rp.yWidth)
			{
				vel.x *= -1;
			}
		}
	}

};



const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform float xOffset,yOffset;"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x + xOffset, aPos.y + yOffset, aPos.z, 1.0);\n"
"}\0";
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";


Ball *ball;
int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwInit();


														 // glfw window creation
														 // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		getchar();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	glewExperimental = true;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		// Problem: glewInit failed, something is seriously wrong.
		char ch;
		std::cout << "glewInit failed: " << glewGetErrorString(err) << std::endl;
		std::cin >> ch;
		exit(1);

	}


	// build and compile our shader program
	// ------------------------------------
	// vertex shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// link shaders
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	
	std::cout << "Player 1   -   Player 2\n";

	RP = new Paddle(0.97f,0.0f);
	LP = new Paddle(-0.97f, 0.0f);

	//for(int i =0;i<5;i++)
		ball = new Ball(0.0f,0.0f,0.04f);
	
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// draw our first triangle
		glUseProgram(shaderProgram);
		
		RP->draw();
		LP->draw();
		
		
		//ball->CheckCollisionWithLeftPaddle(*LP);

		//ball->CheckCollisionWithRightPaddle(*RP);

		for (int i = 0; i < 5; i++)
		{
			ball->draw();

			ball->CheckColWithLPaddle(LP->position, LP->xWidth, LP->yWidth);

			ball->CheckColWithRPaddle(RP->position, RP->xWidth, RP->yWidth);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();


		

	}

	delete ball;
	
	glfwTerminate();
	return 0;
}


void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		LP->position.y += paddleSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		LP->position.y -= paddleSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		RP->position.y += paddleSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		RP->position.y -= paddleSpeed;
	}
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	
	glViewport(0, 0, width, height);
}






#include "glad/glad.h"
#include "GLFW/glfw3.h"


#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include < iostream>
#include <array>
#include <fstream>
#include <sstream>

float SCR_WIDTH = 1920, SCR_HEIGHT = 1080;

class Shader {
public:
	unsigned int ID;

	Shader(const char*& vPath, const char*& fPath)
	{
		std::string vertexCode, fragmentCode;
		std::fstream vertexFile, fragmentFile;

		vertexFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
		fragmentFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);

		vertexFile.open(vPath);
		fragmentFile.open(fPath);

		std::stringstream vertexStream, fragmentStream;

		vertexStream << vertexFile.rdbuf();
		fragmentStream << fragmentFile.rdbuf();

		vertexFile.close();
		fragmentFile.close();

		vertexCode = vertexStream.str();
		fragmentCode = fragmentStream.str();


		const char* vertexSource = vertexCode.c_str();
		const char* fragmentSource = fragmentCode.c_str();

		unsigned vertexShader, fragmentShader;

		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		ID = glCreateProgram();

		glShaderSource(vertexShader, 1, &vertexSource, NULL);
		glShaderSource(fragmentShader, 1, &fragmentSource, NULL);

		glCompileShader(vertexShader);
		glCompileShader(fragmentShader);

		glAttachShader(ID, vertexShader);
		glAttachShader(ID, fragmentShader);

		glLinkProgram(ID);

		checkStatus(vertexShader, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
		checkStatus(fragmentShader, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
		checkStatus(ID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);


		glDetachShader(ID, vertexShader);
		glDetachShader(ID, fragmentShader);
	}

	void use()
	{
		glUseProgram(ID);
	}

	void checkStatus(unsigned int ID, PFNGLGETSHADERIVPROC GetivType, PFNGLGETSHADERINFOLOGPROC GetInfoLogType, unsigned int StatusType)
	{
		int success;
		char infolog[512];

		GetivType(ID, StatusType, &success);
		if (!success) {
			GetInfoLogType(ID, 512, NULL, infolog);
			std::cout << infolog;
		}
	}

	void setMat4(const std::string& name, glm::mat4 value)
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}
	
	void setFloat(const std::string& name, float value)
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
};

void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main()
{
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OPENGL", NULL, NULL);
	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
		return -1;

	std::array<float, 50> vertices = {
		-0.5, 0.5, 0.0,		0.5, 0.0, 1.0,
		0.5, 0.5, 0.0,		1.0, 0.0, 0.0,
		0.5, -0.5, 0.0,		1.0, 1.0, 0.0, 
		-0.5, -0.5, 0.0,	0.0, 1.0, 1.0,

		-0.5, 0.5, -1.0,	0.5, 0.0, 1.0,
		0.5, 0.5, -1.0,		1.0, 0.0, 0.0,
		0.5, -0.5, -1.0,	1.0, 1.0, 0.0,
		-0.5, -0.5, -1.0,	0.0, 1.0, 1.0,

	};

	std::array<int, 36> indices = {
		0, 1, 2,
		3, 2, 0,

		4, 5, 6,
		7, 6, 4,

		4, 0, 1,
		1, 5, 4,

		2, 3, 6,
		6, 7, 3,

		0, 3, 4,
		4, 7, 3,

		1, 2, 6,
		1, 5, 6
	};


	unsigned int VBO, EBO, VAO;
	
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	const char* vertexPath = "Vertex.glsl";
	const char* fragmentPath = "Fragment.glsl";

	Shader shader(vertexPath, fragmentPath);
	shader.use();


	glm::mat4 projection;
	projection = glm::perspective(glm::radians(90.0f), SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0, 0.0, -3.0));

	glEnable(GL_DEPTH_TEST);
	
	while (!glfwWindowShouldClose(window))
	{
		float time = glfwGetTime();
	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, time * glm::radians(50.0f), glm::vec3(sin(time * 2), sin(time * 3), cos(time)));
		model = glm::translate(model, glm::vec3(sin(time), (sin(time * 3) + 1) / 2 * 0.5, (tan(time) + 1) * 2 / 2 * -1));

		shader.setMat4("projection", projection);
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setFloat("time", time);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
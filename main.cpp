#include <iostream>
#include <vector>
#include <string>
#include <map>

// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// load image
extern "C" {
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
}


enum class Format : uint8_t
{
	PNG = 0,
	JPG,
	BMP
};


// declaration

struct Vector4f
{
	union 
	{
		struct
		{
			float x, y, z, a;
		};

		struct
		{
			float data[4];
		};
	};

	Vector4f() : Vector4f(0, 0, 0, 0) {}
	Vector4f(float x, float y, float z, float a)
		:
		x(x), y(y), z(z), a(a)
	{}
};


class Sorter
{
private:
	std::vector<unsigned char> raw_image;
	std::map<std::string, int> image_colors;

	int m_Width;
	int m_Height;

	std::size_t RGBA;
	std::string filename;

public:
	Sorter();
	~Sorter();

	std::string calculate(std::string& filename);

private:
	bool load_image(std::vector<unsigned char>& image, const std::string& filename, int& x, int& y);
	std::string get_image_type(std::string& filename);
};





// definition

Sorter::Sorter()
	:
	RGBA(4),
	m_Width(0),
	m_Height(0)
{}

Sorter::~Sorter() {}


bool Sorter::load_image(std::vector<unsigned char>& image, const std::string& filename, int& x, int& y)
{
	int n;
	unsigned char* data = stbi_load(filename.c_str(), &x, &y, &n, 4);	// change last argument to dynamic var
	if (data != nullptr)
	{
		image = std::vector<unsigned char>(data, data + x * y * 4);	// same here
	}
	stbi_image_free(data);
	return (data != nullptr);
}


std::string Sorter::get_image_type(std::string& filename)
{
	std::string token = filename.substr(1, filename.find("."));
	return token;	// ???
}

std::string Sorter::calculate(std::string& filename)
{
	bool success = load_image(raw_image, filename, m_Width, m_Height);
	if (!success)
	{
		std::cout << "Error loading image !\n";	// return smth ?
	}

	std::cout << "image width = " << m_Width << std::endl;
	std::cout << "image height = " << m_Height << std::endl;

	for (int x = 0; x < m_Width; x++)
	{
		for (int y = 0; y < m_Height; y++)
		{
			std::size_t index = RGBA * (y * m_Width + x);

			std::string rgba = "";
			rgba += std::to_string(raw_image[index + 0]);
			rgba += " ";
			rgba += std::to_string(raw_image[index + 1]);
			rgba += " ";
			rgba += std::to_string(raw_image[index + 2]);
			rgba += " ";
			rgba += std::to_string(raw_image[index + 3]);

			image_colors.find(rgba) != image_colors.end() ? image_colors[rgba]++ : image_colors[rgba] = 0;
		}
	}

	std::string key = "";
	unsigned int current_max = 0;
	for (std::map<std::string, int>::iterator it = image_colors.begin(); it != image_colors.end(); ++it)
	{
		if (it->second > current_max)
		{
			key = it->first;
			current_max = it->second;
		}
	}

	return key;
}

void Input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


int main(int argc, char* argv[])
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Title", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create window!" << std::endl;
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to init GLAD !" << std::endl;
		return -1;
	}

	// shaders

	const char* vs_source = "#version 330 core\n"
		"layout (location = 0) in vec3 pos;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);\n"
		"}";

	const char* fs_source = "#version 330 core\n"
		"out vec4 frag_color;\n"
		"void main()\n"
		"{\n"
		"	frag_color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\n";

	//vertex
	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_source, NULL);
	glCompileShader(vs);

	int s;
	char log[512];
	
	glGetShaderiv(vs, GL_COMPILE_STATUS, &s);
	if (!s)
	{
		glGetShaderInfoLog(vs,	sizeof(log), NULL, log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << log << std::endl;
	}

	//fragment
	unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_source, NULL);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &s);
	if (!s)
	{
		glGetShaderInfoLog(fs, sizeof(log), NULL, log);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << log << std::endl;
	}
	
	//link shaders
	unsigned int shader_program = glCreateProgram();
	glAttachShader(shader_program, vs);
	glAttachShader(shader_program, fs);
	glLinkProgram(shader_program);

	glGetShaderiv(shader_program, GL_LINK_STATUS, &s);
	if (!s)
	{
		glGetShaderInfoLog(shader_program, sizeof(log), NULL, log);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << log << std::endl;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);


	// set up vertex data(buffers etc)

	float vertices[] = 
	{
		 0.5f,  0.5f, 0.0f, // top right
		 0.5f, -0.5f, 0.0f, // bot right
		-0.5f, -0.5f, 0.0f, // bot left
		-0.5f,  0.5f, 0.0f  // top left
	};

	unsigned int indices[] =
	{
		0, 1, 3,	// first
		1, 2, 3		// second
	};
	
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// VAO -> bind and set vertex buffer -> configure vertex attrib
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		// input
		Input(window);

		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// draw
		glUseProgram(shader_program);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// IO events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shader_program);

	(void)argc;
	(void)argv;

	glfwTerminate();

#if 0
	Sorter sorter;
	std::string result = sorter.calculate("../resources/file.jpg");
	
	std::cout << "color: " << result << "\n";
#endif

	return 0;
}
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <array>
#include <iomanip>

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

template<typename T>
void print_array(const T& e)
{
	std::cout << std::setw(3) << e << " ";
}


template<typename T, std::size_t N>
void print_array(const std::array<T, N>& A)
{
	for (const auto& e : A)
		print_array(+e);
	std::cout << "\n";
}


typedef struct vec4
{
	union 
	{
		struct
		{
			uint8_t x, y, z, a;
		};

		struct
		{
			std::array<uint8_t, 4> xyza;
		};
	};

	vec4(uint8_t _x, uint8_t _y, uint8_t _z, uint8_t _a)
		:
		x(_x), y(_y), z(_z), a(_a)
	{}

	//	move semantics (why?)
	vec4(std::array<uint8_t, 4>&& arr)
		:
		xyza(arr)
	{}
	
	vec4() : vec4(0, 0, 0, 0) {}
} vec4_t;


class Sorter
{
private:
	std::vector<uint8_t> raw_image;
	std::map<std::array<uint8_t, 4>, int> image_colors;

	int m_Width;
	int m_Height;

	std::size_t RGBA;
	std::string filename;

public:
	Sorter();
	~Sorter();

	std::array<uint8_t, 4> calculate(const char* filename);

private:
	bool load_image(std::vector<uint8_t>& image, const std::string& filename, int& x, int& y);
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


bool Sorter::load_image(std::vector<uint8_t>& image, const std::string& filename, int& x, int& y)
{
	int n;
	unsigned char* data = stbi_load(filename.c_str(), &x, &y, &n, 4);	// change last argument to dynamic var
	if (data != nullptr)
	{
		image = std::vector<uint8_t>(data, data + x * y * 4);	// same here
	}
	stbi_image_free(data);
	return (data != nullptr);
}


std::string Sorter::get_image_type(std::string& filename)
{
	std::string token = filename.substr(1, filename.find("."));
	return token;	// ???
}

std::array<uint8_t, 4> Sorter::calculate(const char* filename)
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
			
			std::array<uint8_t, 4> foo = { raw_image[index + 0], raw_image[index + 1], raw_image[index + 2], raw_image[index + 3] };
			//std::cout << "color: " << +foo[0] << " " << +foo[1] << " " << +foo[2] << " " << +foo[3] << "\n";
			vec4_t color(std::move(foo));

			image_colors.find(color.xyza) != image_colors.end() ? image_colors[color.xyza]++ : image_colors[color.xyza] = 1;
		}
	}

	std::array<uint8_t, 4> key;
	unsigned int current_max = 0;
	for (std::map<std::array<uint8_t, 4>, int>::iterator it = image_colors.begin(); it != image_colors.end(); ++it)
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
	Sorter sorter;
	printf("magic is happening...\n");
	std::array<uint8_t, 4> uint_color = sorter.calculate("../resources/file.jpg");

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
		"	gl_Position = vec4(pos, 1.0);\n"
		"}\n";

	const char* fs_source = "#version 330 core\n"
		"out vec4 frag_color;\n"
		"uniform vec4 rect_color;\n"
		"void main()\n"
		"{\n"
		"	frag_color = rect_color;\n"
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

	float p = (-1.0f - 1.0f) / 3.f;

	float vertices[] =
	{
		 1.0f,  1.0f, 0.0f,  // top right
		 1.0f,	p,    0.0f,  // bot right
		-1.0f,	p,    0.0f,  // bot left
		-1.0f,  1.0f, 0.0f,  // top left
	};

	// rectangles
	unsigned int indices[] =
	{
		// first
		0, 1, 3,
		1, 2, 3,
		// second

		// third
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

		GLint color_location = glGetUniformLocation(shader_program, "rect_color");
		glUniform4f(color_location, (uint_color[0] / 255.f), (uint_color[1] / 255.f), (uint_color[2] / 255.f), (uint_color[3] / 255.f));

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

	glfwTerminate();

	(void)argc;
	(void)argv;
	return 0;
}
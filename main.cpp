#include <iostream>
#include <vector>
#include <string>
#include <map>


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

	std::string calculate(std::string filename);

private:
	bool load_image(std::vector<unsigned char>& image, const std::string& filename, int& x, int& y);
	std::string get_image_type(std::string filename);
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


std::string Sorter::get_image_type(std::string filename)
{
	std::string token = filename.substr(1, filename.find("."));
	return token;	// ???
}

std::string Sorter::calculate(std::string filename)
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

			if (image_colors.find(rgba) != image_colors.end())
			{
				image_colors[rgba]++;
			}
			else
			{
				image_colors[rgba] = 0;
			}
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


int main(int argc, char* argv[])
{
	Sorter sorter;
	std::string result = sorter.calculate("../resources/file.jpg");
	
	std::cout << "color: " << result << "\n";

	system("PAUSE");

	(void)argc;
	(void)argv;
	return 0;
}
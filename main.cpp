#include <iostream>
#include <vector>
#include <array>
#include <iomanip>

extern "C" {
	#define STB_IMAGE_IMPLEMENTATION
	#include "stb_image.h"
}


template <typename T>
void print_array(const T& e)
{
	std::cout << std::setw(3) << e << ' ';
}

template<typename T, std::size_t N>
void print_array(const std::array<T, N>& A)
{
	for (const auto& e : A)
		print_array(e);
	std::cout << '\n';
}


bool load_image(std::vector<unsigned char>& image, const std::string& filename, int& x, int& y)
{
	int n;
	unsigned char* data = stbi_load(filename.c_str(), &x, &y, &n, 4);
	if (data != nullptr)
	{
		image = std::vector<unsigned char>(data, data + x * y * 4);
	}
	stbi_image_free(data);
	return (data != nullptr);
}


int main(int argc, char* argv[])
{
	std::string filename = "../resources/file.jpg";
	const std::size_t RGBA = 4;
	int width, height;

	std::vector<unsigned char> image;
	std::vector<std::array<unsigned int, RGBA>> image_rgba;	// ???

	bool success = load_image(image, filename, width, height);
	if (!success)
	{
		std::cout << "Error loading image!\n";
		return 1;
	}

	std::cout << "image width = " << width << '\n';
	std::cout << "image height = " << height << '\n';

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			std::size_t index = RGBA * (y * width + x);

			std::array<unsigned int, RGBA> rgba = {
				static_cast<int>(image[index + 0]),
				static_cast<int>(image[index + 1]),
				static_cast<int>(image[index + 2]),
				static_cast<int>(image[index + 3])
			};

			image_rgba.push_back(rgba);
		}
	}

	for (std::vector<std::array<unsigned int, RGBA>>::iterator it = image_rgba.begin(); it != image_rgba.end(); ++it)
	{
		print_array(*it);
	}


	system("PAUSE");
	return 0;
}
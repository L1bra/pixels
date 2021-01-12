#include <iostream>
#include <vector>
#include <array>

extern "C" {
	#define STB_IMAGE_IMPLEMENTATION
	#include "stb_image.h"
}


template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vector)
{
	for (auto element : vector)
	{
		os << element << " ";
	}
	return os;
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
	std::string filename = "../resources/filename.jpg";
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

	int x = 3;
	int y = 4;
	std::size_t i = 0;	// ???
	std::size_t index = RGBA * (y * width + x);

	// ???
	for (std::vector<unsigned char>::iterator it = image.begin(); it != image.end(); i++, ++it)
	{
		std::array<unsigned int, RGBA> rgba = { static_cast<int>(image[index + (i + 0)]), 
												static_cast<int>(image[index + (i + 1)]),
												static_cast<int>(image[index + (i + 2)]),
												static_cast<int>(image[index + (i + 3)])};
		image_rgba.push_back(rgba);			    
	}
	

	//std::cout << "RGBA pixel(" << x << ", " << y << "):" 
	//	<< static_cast<int>(image[index + 0]) << " "
	//	<< static_cast<int>(image[index + 1]) << " "
	//	<< static_cast<int>(image[index + 2]) << " "
	//	<< static_cast<int>(image[index + 3]) << "\n";

	system("PAUSE");
	return 0;
}
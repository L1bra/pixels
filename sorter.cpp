#include "sorter.h"

extern "C" {
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
}



Sorter::Sorter()
{
	RGBA = 4;	// ???

}

Sorter::~Sorter()
{
}


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


bool Sorter::calculate(std::string filename)
{
	bool success = load_image(image, filename, m_Width, m_Height);
	if (!success)
	{
		std::cout << "Error loading image !\n";
		return 1;
	}

	std::cout << "image width = " << m_Width << std::endl;
	std::cout << "image height = " << m_Height << std::endl;

	for (int x = 0; x < m_Width; x++)
	{
		for (int y = 0; y < m_Height; y++)
		{
			std::size_t index = RGBA * (y * m_Width + x);

			std::array<unsigned int, 4> rgba = {
				static_cast<int>(image[index + 0]),
				static_cast<int>(image[index + 1]),
				static_cast<int>(image[index + 2]),
				static_cast<int>(image[index + 3])
			};

			image_rgba.push_back(rgba);
		}
	}

	for (std::vector<std::array<unsigned int, 4>>::iterator it = image_rgba.begin(); it != image_rgba.end(); ++it)
	{
		print_array(*it);
	}
}
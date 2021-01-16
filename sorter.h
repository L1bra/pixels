#ifndef SORTER_H
#define SORTER_H

#include <iostream>
#include <vector>
#include <array>
#include <iomanip>


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


class Sorter
{
private:
	int m_Width;
	int m_Height;

	std::size_t RGBA;
	std::string filename;

	std::vector<unsigned char> image;
	std::vector<std::array<unsigned int, 4>> image_rgba;
public:
	Sorter();
	~Sorter();

	bool calculate(std::string filename);

	
private:
	bool load_image(std::vector<unsigned char>& image, const std::string& filename, int& x, int& y);
};


#endif // SORTER_H
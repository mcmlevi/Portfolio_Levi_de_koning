#pragma once
struct Image
{
	Image(const char* imagePath = "", int imgWidth = 0, int imgHeight = 0, int channels = 3):width(imgWidth),height(imgHeight)
	{
		stbi_set_flip_vertically_on_load(true);
		image = stbi_load(imagePath, &width, &height, &channels, STBI_rgb);
	}
	~Image();
	unsigned char* image;
	int width;
	int height;
};

inline Image::~Image()
{
	stbi_image_free(image);
}

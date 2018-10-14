#ifndef TEXTURE_H
#define TEXTURE_H

#include "HelperFunctions.h"
#include <stb_image.h>

class Texture {
private:
	VkImage textureImage;
	VkImageView textureImageView;
	VkDeviceMemory textureImageMemory;
	uint32_t mipLevels;

public:
	Texture();
	~Texture();

	void createTextureImage(std::string texture_path, helpers::VulkanData vulkanData);
	void createTextureImageView(helpers::VulkanData vulkanData, uint32_t mipLevels);
};

#endif


#ifndef TEXTURE_H
#define TEXTURE_H

#include "HelperFunctions.h"
#include <stb_image.h>

class Texture {
private:
	VkImage textureImage;
	VkImageView textureImageView;
	VkDeviceMemory textureImageMemory;

public:
	Texture();
	~Texture();

	void createTextureImage(std::string texture_path, uint32_t mipLevels, helpers::VulkanData vulkanData);
	void createTextureImageView(helpers::VulkanData vulkanData, uint32_t mipLevels);
};

#endif


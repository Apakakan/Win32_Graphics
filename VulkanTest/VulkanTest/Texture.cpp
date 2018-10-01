#include "Texture.h"

Texture::Texture() {

}
Texture::~Texture() {

}

void Texture::createTextureImage(std::string texture_path, uint32_t mipLevels, helpers::VulkanData vulkanData) {
	int texWidth, texHeight, texChannels;
	VkDevice* device = vulkanData.device;

	stbi_uc* pixels = stbi_load(texture_path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels) {
		throw std::runtime_error("failed to load image");
	}

	mipLevels = static_cast<uint32_t> (std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	helpers::createBuffer(vulkanData, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void * data;
	vkMapMemory(*device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<uint32_t>(imageSize));
	vkUnmapMemory(*device, stagingBufferMemory);

	stbi_image_free(pixels);

	helpers::createImage(vulkanData, texWidth, texHeight, mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

	helpers::transitionImageLayout(vulkanData, textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);

	helpers::copyBufferToImage(vulkanData, stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

	//transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,mipLevels);
	helpers::GenerateMipMaps(vulkanData, textureImage, VK_FORMAT_R8G8B8A8_UNORM, texWidth, texHeight, mipLevels, mipLevels);

	vkDestroyBuffer(*device, stagingBuffer, nullptr);
	vkFreeMemory(*device, stagingBufferMemory, nullptr);

	createTextureImageView(vulkanData, mipLevels);
}

void Texture::createTextureImageView(helpers::VulkanData vulkanData, uint32_t mipLevels) {
	textureImageView = helpers::createImageView(vulkanData, textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
}
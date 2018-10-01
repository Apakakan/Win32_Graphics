#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H
#include <vulkan/vulkan.h>

#include <array>
namespace helpers {

	struct VulkanData {
		VkDevice* device;
		VkPhysicalDevice* physicalDeviec;
		VkQueue* graphicsQueue;
		VkCommandPool* commandPool;
	};
	bool hasStencilComponent(VkFormat format);
	uint32_t findMemoryType(VkPhysicalDevice* physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	VkCommandBuffer beginSingletTimeCommands(VulkanData vulkanData);
	void endSingleTimeCommands(VulkanData vulkandata, VkCommandBuffer commandBuffer);
	void createBuffer(VulkanData vulkanData, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VulkanData vulkanData, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void createImage(VulkanData vulkanData, uint32_t width, uint32_t heiht, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	void transitionImageLayout(VulkanData vulkanData, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
	void copyBufferToImage(VulkanData vulkanData, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void GenerateMipMaps(VulkanData vulkanData, VkImage image, VkFormat imageFormat, int32_t texWidth, int texHeight, uint32_t miplevels, uint32_t baseMipLevels);
	VkImageView createImageView(VulkanData vulkanData, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

	
};

#endif



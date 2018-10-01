#include "Model.h"

Model::Model() {

}
Model::~Model() {

}

void Model::LoadModel(std::string model_path, helpers::VulkanData vulkanData) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, model_path.c_str())) {
		throw std::runtime_error(err);
	}

	//std::unordered_map<Vertex, uint32_t> uniqueVertex = {};
	std::unordered_map<Vertex, uint32_t> uniqueVertex = {};
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex = {};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			if (uniqueVertex.count(vertex) == 0) {
				uniqueVertex[vertex] = static_cast<uint32_t>(verticies.size());
				verticies.push_back(vertex);
			}

			indices.push_back(uniqueVertex[vertex]);
		}
	}
	createVertexBuffer(vulkanData);
	createIndexBuffer(vulkanData);
}

void Model::createVertexBuffer(helpers::VulkanData vulkanData) {
	VkDevice* device = vulkanData.device;

	VkDeviceSize bufferSize = sizeof(verticies[0]) * verticies.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	helpers::createBuffer(vulkanData, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(*device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, verticies.data(), (size_t)bufferSize);
	vkUnmapMemory(*device, stagingBufferMemory);

	helpers::createBuffer(vulkanData, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBufer, vertexBufferMemory);
	helpers::copyBuffer(vulkanData, stagingBuffer, vertexBufer, bufferSize);

	vkDestroyBuffer(*device, stagingBuffer, nullptr);
	vkFreeMemory(*device, stagingBufferMemory, nullptr);
}
void Model::createIndexBuffer(helpers::VulkanData vulkanData) {

	VkDevice* device = vulkanData.device;
	VkPhysicalDevice* physicalDeivce = vulkanData.physicalDeviec;

	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	helpers::createBuffer(vulkanData, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;

	vkMapMemory(*device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(*device, stagingBufferMemory);

	helpers::createBuffer(vulkanData, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	helpers::copyBuffer(vulkanData, stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(*device, stagingBuffer, nullptr);
	vkFreeMemory(*device, stagingBufferMemory, nullptr);
}

glm::vec3 Model::getPosition() {
	return position;
}
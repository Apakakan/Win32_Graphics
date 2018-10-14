#ifndef OBJECT_H
#define OBJECT_H

#include "Model.h"
#include "Texture.h"

class Object {
private:
	Model model;
	Texture texture;

	glm::vec3 position;

public:
	Object();
	~Object();

	void loadTextureAndModel(helpers::VulkanData vulkanData, std::string modelPath, std::string texturePath);

	Model* getModel();
	Texture* getTexture();
	glm::vec3 getPosition();
};

#endif

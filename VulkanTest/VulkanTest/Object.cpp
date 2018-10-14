#include "Object.h"

Object::Object() {

}
Object::~Object() {

}

void Object::loadTextureAndModel(helpers::VulkanData vulkanData, std::string modelPath, std::string texturePath) {
	model.LoadModel(modelPath, vulkanData);
	texture.createTextureImage(texturePath, vulkanData);
}

Model* Object::getModel() {
	return &model;
}
Texture* Object::getTexture() {
	return &texture;
}
glm::vec3 Object::getPosition() {
	return position;
}
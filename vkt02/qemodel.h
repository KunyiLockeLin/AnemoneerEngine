#pragma once

#include "qeheader.h"


struct UniformBufferObject {
	QeMatrix4x4f model;
	QeMatrix4x4f view;
	QeMatrix4x4f proj;
};

class QeModel
{
private:
public:
	QeVector3f pos;
	float face;
	float up;
	QeVector3f size;

	const std::string MODEL_PATH = "../data/models/plane.obj";
	//const std::string MODEL_PATH = "../data/models/tri.obj";
	//const std::string MODEL_PATH = "../data/models/box.obj";
	//const std::string MODEL_PATH = "../data/models/table.obj";
	//const std::string TEXTURE_PATH = "../data/textures/table.bmp";
	//const std::string MODEL_PATH = "../data/models/billboard.obj";
	//const std::string TEXTURE_PATH = "../data/textures/billboard.bmp";
	//const std::string MODEL_PATH = "../data/models/chalet.obj";
	//const std::string TEXTURE_PATH = "../data/textures/chalet.bmp";

	QeAssetModel* modelData;

	VkBuffer uniformBuffer;
	VkDeviceMemory uniformBufferMemory;

	void loadModel();
	void createUniformBuffer();
	void updateDescriptorSet();
	void updateUniformBuffer();
	void update();
	
public: 
	QeModel() {}
	~QeModel() {}
	void init();
	void cleanup();
	void setPosFaceUpSize(QeVector3f _pos, float _face, float _up, QeVector3f _size);
	void move(QeVector3f _pos);
	void setPosition(QeVector3f _pos);
	void rotateFace( float _face);
	void setFace(float _face);
	void rotateUp(float _up);
	void setUp(float _up);
	void enlarge(QeVector3f _size);
	void setSize(QeVector3f _size);
	QeMatrix4x4f getMatModel();
};


#pragma once

#include "qeheader.h"


struct QeUniformBufferObject {
	QeMatrix4x4f model;
	QeMatrix4x4f view[MAX_VIEWPORT_NUM];
	QeMatrix4x4f proj[MAX_VIEWPORT_NUM];
	QeMatrix4x4f normal[MAX_VIEWPORT_NUM];
	QeVector4f cameraPos[MAX_VIEWPORT_NUM];
	QeVector4f ambientColor;
	QeVector4f param; // 1:viewportNum, 2:billboardType,
};

class QeModel
{
public:

	unsigned char actionState; // 0000 stop, 0001 play, 0010 pause, 0100 auto next, 1000 auto replay
	unsigned char cuurentAction;
	float cuurentFrames;

	QeVector3f pos;
	float face;
	float up;
	QeVector3f size;
	int speed=0;

	QeAssetModel* modelData;

	VkDescriptorSet descriptorSet;
	QeUniformBufferObject ubo;
	VkBuffer uboBuffer;
	VkDeviceMemory uboBufferMemory;
	VkBuffer lightBuffer;
	VkDeviceMemory lightBufferMemory;

	VkPipeline graphicsPipeline;
	VkDescriptorPool descriptorPool;
	QeAssetXML* initProperty;

	void createDescriptorBuffer();
	void updateUniformBuffer();
	virtual void update(float time);
	
	QeModel(QeObjectMangerKey& _key) {}
	~QeModel();

	void init(QeAssetXML* _property);
	void setPosFaceUpSize(QeVector3f _pos, float _face, float _up, QeVector3f _size);
	void move(QeVector3f _pos);
	void setPosition(QeVector3f _pos);
	void rotateFace( float _face);
	void setFace(float _face);
	void rotateUp(float _up);
	void setUp(float _up);
	void enlarge(QeVector3f _size);
	void setSize(QeVector3f _size);
	virtual void setMatModel();
	void cleanupSwapChain();
	void createSwapChain();

	void playAction();
	void playAction( unsigned int actionID, unsigned char playType );
	void pauseAction();
	void stopAction();
	void updateAction();

};


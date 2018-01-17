#pragma once

#include "qeheader.h"


struct QeUniformBufferObject {
	QeMatrix4x4f model;
	QeMatrix4x4f view[MAX_VIEWPORT_NUM];
	QeMatrix4x4f proj[MAX_VIEWPORT_NUM];
	QeMatrix4x4f normal[MAX_VIEWPORT_NUM];
	QeMatrix4x4f joints[MAX_JOINT_NUM];
	QeVector4f	cameraPos[MAX_VIEWPORT_NUM];
	QeVector4f	ambientColor;
	QeVector4f	param; // 1:viewportNum, 2:billboardType,
};

enum QeActionType {
	eActionTypeOnce,
	eActionTypeReplay,
	eActionTypeNext,
};

enum QeActionState {
	eActionStateStop,
	eActionStatePlay,
	eActionStatePause,
};

class QeModel
{
public:
	QeActionState	actionState;
	QeActionType	actionType;
	unsigned char	currentActionID;
	unsigned int	currentActionFrame;
	float			currentActionTime;
	size_t id;
	QeVector3f pos;
	float face;
	float up;
	QeVector3f size;
	int speed=0;

	size_t attachID;
	const char* attachSkeletonName;

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

	bool setAction(unsigned int actionID, QeActionType playType);
	void actionPlay();
	void actionPause();
	void actionStop();
	void updateAction(float time);
	void setChildrenJointTransform(QeMatrix4x4f* jointsTransform, QeDataJoint& joint, QeMatrix4x4f &parentTransform);
};


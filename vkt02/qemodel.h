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
	QeVector4f	param; // 1:viewportNum, 2:billboardType, 3:bCubemap
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
	QeActionState	actionState = eActionStateStop;
	QeActionType	actionType = eActionTypeOnce;
	unsigned char	currentActionID = 0;
	unsigned int	currentActionFrame = 0;
	float			currentActionTime = 0.f;
	QeMatrix4x4f	joints[MAX_JOINT_NUM];
	float actionSpeed = 0.f;

	//QePipelineType pipelineType;
	int id = 0;
	QeVector3f pos;
	float face = 0.f;
	float up = 0.f;
	QeVector3f size;
	int speed = 0;
	bool bShow = true;
	bool bCullingShow = true;
	int cullingDistance = 0;
	int attachID = 0;
	int cubeMapID = 0;
	const char* attachSkeletonName = nullptr;

	QeAssetModel* modelData = nullptr;
	QeAssetMaterial * pMaterial = nullptr;

	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
	VkPipeline pipeline = VK_NULL_HANDLE;
	VkPipeline normalPipeline = VK_NULL_HANDLE;

	QeAssetShader normalShader;

	QeUniformBufferObject ubo;
	QeVKBuffer uboBuffer;
	QeAssetXML* initProperty = nullptr;

	void setShow(bool b);
	void updateUniformBuffer();
	virtual void updateShowByCulling();
	virtual void updateRender(float time);
	virtual void updateCompute(float time);

	QeModel(QeObjectMangerKey& _key) {}
	~QeModel();

	virtual void init(QeAssetXML* _property);
	void setPosFaceUpSize(QeVector3f& _pos, float _face, float _up, QeVector3f& _size);
	void move(QeVector3f& _pos);
	void setPosition(QeVector3f& _pos);
	void rotateFace( float _face);
	void setFace(float _face);
	void rotateUp(float _up);
	void setUp(float _up);
	void enlarge(QeVector3f& _size);
	void setSize(QeVector3f& _size);
	virtual void setMatModel();
	void cleanupPipeline();
	virtual void createGraphicsPipeline();
 
	bool setAction(unsigned int actionID, QeActionType playType);
	void actionPlay();
	void actionPause();
	void actionStop();
	void updateAction(float time);
	void setChildrenJointTransform( QeDataJoint& joint, QeMatrix4x4f &parentTransform);
	QeMatrix4x4f getAttachMatrix( const char* attachSkeletonName );
	virtual void updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer);
};
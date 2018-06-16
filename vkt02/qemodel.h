#pragma once

#include "qeheader.h"


struct QeDataModel {
	QeMatrix4x4f model;
	QeMatrix4x4f joints[MAX_JOINT_NUM];
	QeVector4f	param; // 0: billboardType / bCubemap, 1: particleFollow(2:follow 1:unfollow 0:none)
	QeDataMaterial material;
};

enum QeModelType {
	eModel_Model = 0,
	eModel_Billboard = 1,
	eModel_Line = 2,
	eModel_Cube = 3,
	eModel_Particle = 4,
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

struct QeDataModelViewport {
	QeMatrix4x4f normal;
};

struct QeDataModelShader {
	QeDataModelViewport data;

	QeVKBuffer buffer;
	QeDataDescriptorSet descriptorSet;

	QeDataModelShader():buffer(eBuffer_uniform), descriptorSet(eDescriptorSetLayout_Model) {}
};

class QeModel:public QeBase
{
public:

	QeModel(QeObjectMangerKey& _key, QeModelType _type = eModel_Model) :QeBase(_key), modelType(_type), modelBuffer(eBuffer_uniform){}
	~QeModel();

	QeModelType modelType = eModel_Model;
	QeActionState	actionState = eActionStateStop;
	QeActionType	actionType = eActionTypeOnce;
	uint16_t		currentActionID = 0;
	unsigned int	currentActionFrame = 0;
	float			currentActionTime = 0.f;
	QeMatrix4x4f	joints[MAX_JOINT_NUM];
	float actionSpeed = 0.f;

	//QePipelineType pipelineType;
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
	bool bAlpha = false;

	QeAssetModel* modelData = nullptr;
	QeAssetMaterial* pMaterial = nullptr;
	VkShaderModule computeShader = VK_NULL_HANDLE;

	QeDataModel bufferData;
	QeVKBuffer modelBuffer;

	std::vector<QeDataModelShader*> shaderData;
	QeDataGraphicsPipeline* graphicsPipeline = nullptr;
	QeDataGraphicsPipeline* normalPipeline = nullptr;
	VkPipeline computePipeline = VK_NULL_HANDLE;

	QeAssetShader normalShader;

	void setShow(bool b);
	void updateBuffer();
	virtual void updateShowByCulling();
	virtual void updateRender(float time);
	virtual void updateCompute(float time);

	virtual void init(QeAssetXML* _property);
	virtual void setProperty();
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
	void updateShaderData();
	virtual QeDataDescriptorSetModel createDescriptorSetModel(int index);
	virtual void createPipeline();
 
	bool setAction(unsigned int actionID, QeActionType playType);
	void actionPlay();
	void actionPause();
	void actionStop();
	void updateAction(float time);
	void setChildrenJointTransform( QeDataJoint& joint, QeMatrix4x4f &parentTransform);
	QeMatrix4x4f getAttachMatrix( const char* attachSkeletonName );
	std::vector<VkDescriptorSet> getDescriptorSets(int index);
	virtual void updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer);
	virtual void updateComputeCommandBuffer(VkCommandBuffer& drawCommandBuffer) {}
};
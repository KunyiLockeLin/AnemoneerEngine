#pragma once

#include "qeheader.h"


struct QeDataModel {
	QeMatrix4x4f	model;
	QeMatrix4x4f	joints[MAX_JOINT_NUM];
	QeVector4f		param; // 0: bCubemap
	QeDataMaterial	material;
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

/*struct QeDataModelViewport {
	QeMatrix4x4f normal;
};

struct QeDataModelShader {
	//QeDataModelViewport data;
	//QeVKBuffer buffer;
	QeDataDescriptorSet descriptorSet;

	QeDataModelShader():buffer(eBuffer_uniform), descriptorSet(eDescriptorSetLayout_Model) {}
};
*/

class QeModel:public QePoint
{
public:

	QeModel(QeObjectMangerKey& _key, QeObjectType _type = eObject_Model):QePoint(_key, _type), 
		modelBuffer(eBuffer_uniform), descriptorSet(eDescriptorSetLayout_Model) {}
	//~QeModel(){}

	QeActionState	actionState;
	QeActionType	actionType;
	QeMatrix4x4f	joints[MAX_JOINT_NUM];
	int				currentActionID;
	int				currentActionFrame;
	float			currentActionTime;
	float			actionSpeed;

	QeVector3f size;
	float face;
	float up;
	int speed;
	bool bShow;
	int cullingDistance;
	int cubemapOID;
	bool bAlpha;
	bool bFixSize;
	int cameraOID;

	QeMatrix4x4f parentModel;

	QeAssetModel* modelData = nullptr;
	QeAssetMaterial* mtlData = nullptr;
	QeAssetGraphicsShader graphicsShader;
	QeAssetGraphicsShader normalShader;
	VkShaderModule computeShader = VK_NULL_HANDLE;

	QeDataModel bufferData;
	QeVKBuffer modelBuffer;

	//std::vector<QeDataModelShader*> shaderData;
	QeDataDescriptorSet descriptorSet;
	QeDataGraphicsPipeline* graphicsPipeline = nullptr;
	QeDataGraphicsPipeline* normalPipeline = nullptr;
	VkPipeline computePipeline = VK_NULL_HANDLE;

	virtual void init(QeAssetXML* _property, int _parentOID);
	virtual void setProperty();
	
	void setShow(bool b);
	void updateBuffer();
	virtual bool isShowByCulling(QeCamera* camera);
	//virtual void updateRender(float time) {}
	virtual void updateCompute(float time);

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
	void updateShaderData();
	virtual QeDataDescriptorSetModel createDescriptorSetModel();
	virtual void createPipeline();
 
	bool setAction(unsigned int actionID, QeActionType playType);
	void actionPlay();
	void actionPause();
	void actionStop();
	void updateAction(float time);
	void setChildrenJointTransform( QeDataJoint& joint, QeMatrix4x4f &parentTransform);
	virtual QeMatrix4x4f getAttachMatrix( const char* attachSkeletonName);
	std::vector<VkDescriptorSet> getDescriptorSets(QeDataDescriptorSet* commonDescriptorSet);
	virtual void updateDrawCommandBuffer(VkCommandBuffer& commandBuffer, QeCamera* camera, QeDataDescriptorSet* commonDescriptorSet);
	virtual void updateComputeCommandBuffer(VkCommandBuffer& commandBuffer, QeCamera* camera, QeDataDescriptorSet* commonDescriptorSet) {}
};
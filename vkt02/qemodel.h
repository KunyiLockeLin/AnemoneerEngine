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

class QeModel:public QeComponent
{
public:

	QeModel(QeObjectMangerKey& _key) :QeComponent(_key), modelBuffer(eBuffer_uniform), descriptorSet(eDescriptorSetLayout_Model) {}
	//~QeModel(){}

	virtual void initialize(QeAssetXML* _property, QeObject* _owner);
	virtual void clear();
	virtual void update1();
	virtual void update2();

	// QeMaterial, QeAnimation
	QeAssetModel* modelData = nullptr;
	QeAssetMaterial* materialData = nullptr;
	QeAssetGraphicsShader graphicsShader;
	QeAssetGraphicsShader normalShader;
	QeAssetGraphicsShader outlineShader;
	QeDataGraphicsPipeline graphicsPipeline;

	VkShaderModule computeShader = VK_NULL_HANDLE;

	QeDataDescriptorSet descriptorSet;

	VkPipeline computePipeline = VK_NULL_HANDLE;

	QeDataModel bufferData;
	QeVKBuffer modelBuffer;

	virtual QeDataDescriptorSetModel createDescriptorSetModel();
	virtual void createPipeline();
	virtual bool isShowByCulling(QeCamera* camera);

	virtual void updateDrawCommandBuffer(QeDataDrawCommand* command);
	virtual void updateComputeCommandBuffer(VkCommandBuffer& commandBuffer, QeCamera* camera, QeDataDescriptorSet* commonDescriptorSet) {}
	void updateShaderData();


	/*QeActionState	actionState;
	QeActionType	actionType;
	QeMatrix4x4f	joints[MAX_JOINT_NUM];
	int				currentActionID;
	int				currentActionFrame;
	float			currentActionTime;
	float			actionSpeed;

	bool bShow;
	QeVector3f size;
	float face;
	float up;
	int speed;
	int cullingDistance;
	//int cubemapOID;
	QeVKImage* cubemapImage;
	VkExtent2D renderSize;
	bool bFixSize;
	int cameraOID;

	QeMatrix4x4f parentModel;

	QeAssetModel* modelData = nullptr;
	QeAssetMaterial* mtlData = nullptr;
	QeAssetGraphicsShader graphicsShader;
	QeAssetGraphicsShader normalShader;
	QeAssetGraphicsShader outlineShader;
	QeDataGraphicsPipeline graphicsPipeline;

	VkShaderModule computeShader = VK_NULL_HANDLE;

	QeDataModel bufferData;
	QeVKBuffer modelBuffer;

	//std::vector<QeDataModelShader*> shaderData;
	QeDataDescriptorSet descriptorSet;

	VkPipeline computePipeline = VK_NULL_HANDLE;

	//virtual void init(QeAssetXML* _property, int _parentOID);
	//virtual void setProperty();
	
	void setShow(bool b);
	void updateBuffer();
	virtual bool isShowByCulling(QeCamera* camera);
	virtual void update1();

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
	virtual void updateDrawCommandBuffer(QeDataDrawCommand* command);
	virtual void updateComputeCommandBuffer(VkCommandBuffer& commandBuffer, QeCamera* camera, QeDataDescriptorSet* commonDescriptorSet) {}
	*/
};
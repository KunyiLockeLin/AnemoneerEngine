#pragma once

#include "qeheader.h"


struct QeDataModel {
	QeMatrix4x4f	model;
	QeMatrix4x4f	joints[MAX_JOINT_NUM];
	QeVector4f		param; // 0: bColorMap, 1: bCubeMap, 2: bNormalMap, 3: outlineWidth
	QeDataMaterial	material;
};

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
	const char* shaderKey;
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
	int materialOID;
	bool bUpdateMaterialOID;
	bool bRotate=true;
	bool b2D=false;

	virtual QeDataDescriptorSetModel createDescriptorSetModel();
	virtual void createPipeline();
	virtual bool isShowByCulling(QeCamera* camera);

	virtual void updateDrawCommandBuffer(QeDataDrawCommand* command);
	virtual void updateComputeCommandBuffer(VkCommandBuffer& commandBuffer, QeCamera* camera, QeDataDescriptorSet* commonDescriptorSet) {}
};
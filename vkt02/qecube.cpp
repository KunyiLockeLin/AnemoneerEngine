#include "qeheader.h"


void QeCube::init(QeAssetXML* _property) {

	initProperty = _property;

	modelData = AST->getModel("cube", true);
	pMaterial = AST->getMaterialImage(AST->getXMLValue(_property, 1, "image"), true);

	AST->setShader(pMaterial->shader, initProperty, AST->getXMLNode(3, AST->CONFIG, "defaultShader", "cubemap"));
	setProperty();
}

QeDataDescriptorSetModel QeCube::createDescriptorSetModel(int index) {
	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;
	descriptorSetData.cubeMapImageViews = pMaterial->image.pCubeMap->view;
	descriptorSetData.cubeMapSamplers = pMaterial->image.pCubeMap->sampler;
	return descriptorSetData;
}

void QeCube::createPipeline() {
	graphicsPipeline = VK->createGraphicsPipeline(&pMaterial->shader, eGraphicsPipeLine_Triangle, bAlpha);
}

void QeCube::updateShowByCulling() {
	if (!bShow) return;
	QeCamera* camera = VP->getTargetCamera();
	bool _bCullingShow = true;
	float dis = MATH->distance(pos, camera->pos);

	if (cullingDistance>0) {
		if (dis > cullingDistance) _bCullingShow = false;
	}
	else {
		if (dis > camera->cullingDistance) _bCullingShow = false;
	}

	if (_bCullingShow != bCullingShow) {
		bCullingShow = _bCullingShow;
		VP->bUpdateDrawCommandBuffers = true;
	}
}
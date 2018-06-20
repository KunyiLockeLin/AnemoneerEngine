#include "qeheader.h"


void QeCubemap::init(QeAssetXML* _property, int _parentOID) {

	QePoint::init(_property,  _parentOID);

	modelData = AST->getModel("cube", true);
	mtlData = AST->getMaterialImage(AST->getXMLValue(editProperty, 1, "image"), true);
	bufferData.material = mtlData->value;
	AST->setGraphicsShader(graphicsShader, editProperty, "cubemap");
}

QeDataDescriptorSetModel QeCubemap::createDescriptorSetModel(int index) {
	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;
	descriptorSetData.cubeMapImageViews = mtlData->image.pCubeMap->view;
	descriptorSetData.cubeMapSamplers = mtlData->image.pCubeMap->sampler;
	return descriptorSetData;
}

void QeCubemap::updateShowByCulling() {
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
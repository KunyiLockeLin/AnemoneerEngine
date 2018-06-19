#include "qeheader.h"


void QeLight::init(QeAssetXML* _property) {

	QeBase::init(_property);
	bufferData.pos = QeVector4f(0, 0, 0, 1);
	bufferData.dir = QeVector4f(1, 1, 1, 1);
	bufferData.color = QeVector4f(1, 1, 1, 1);
	bufferData.param = QeVector4f(0, 1, 1, 1);
	rotateCenter = QeVector3f(0, 0, 0);
	speed = 30;

	billboard = nullptr;
	if (_property == nullptr) return;

	initProperty = _property;
	const char* c;
	AST->getXMLiValue(&id, initProperty, 1, "id");

	AST->getXMLfValue(&bufferData.color.x, initProperty, 1, "r");
	AST->getXMLfValue(&bufferData.color.y, initProperty, 1, "g");
	AST->getXMLfValue(&bufferData.color.z, initProperty, 1, "b");

	AST->getXMLfValue(&pos.x, initProperty, 1, "posX");
	AST->getXMLfValue(&pos.y, initProperty, 1, "posY");
	AST->getXMLfValue(&pos.z, initProperty, 1, "posZ");
	bufferData.pos = pos;

	AST->getXMLfValue(&rotateCenter.x, initProperty, 1, "rotateCenterX");
	AST->getXMLfValue(&rotateCenter.y, initProperty, 1, "rotateCenterY");
	AST->getXMLfValue(&rotateCenter.z, initProperty, 1, "rotateCenterZ");

	AST->getXMLfValue(&bufferData.dir.x, initProperty, 1, "dirX");
	AST->getXMLfValue(&bufferData.dir.y, initProperty, 1, "dirY");
	AST->getXMLfValue(&bufferData.dir.z, initProperty, 1, "dirZ");

	AST->getXMLfValue(&speed, initProperty, 1, "speed");

	AST->getXMLfValue(&bufferData.param.x, initProperty, 1, "type");
	AST->getXMLfValue(&bufferData.param.y, initProperty, 1, "intensity");
	AST->getXMLfValue(&bufferData.param.z, initProperty, 1, "coneAngle");

	AST->getXMLbValue(&bShow, initProperty, 1, "show");

	billboard = OBJMGR->getBillboard(id, initProperty);
	//if (billboard->pMaterial->type == eMaterialPhong)
	//	billboard->pMaterial->value.phong.diffuse = bufferData.color;
	//else if (billboard->pMaterial->type == eMaterialPBR)
	//	billboard->pMaterial->value.pbr.baseColor = bufferData.color;
	billboard->bufferData.material.baseColor = bufferData.color;
	bufferData.pos;
	pos = bufferData.pos;
	billboard->setShow(bShow);
	billboard->pos = bufferData.pos;

	bUpdateBuffer = true;

}

void QeLight::updateCompute(float time) {

	if (speed != 0) {
		float angle = -time * speed;
		QeMatrix4x4f mat;
		mat *= MATH->rotateZ(angle);
		QeVector4f pos2 = bufferData.pos - rotateCenter;
		pos2 = mat* pos2;
		bufferData.pos = pos2 + rotateCenter;

		pos = bufferData.pos;
		billboard->setShow(bShow);
		billboard->pos = bufferData.pos;
		
		bUpdateBuffer = true;
	}
}

void QeLight::updateRender(float time) {}
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
	const char* c = AST->getXMLValue(_property, 1, "id");
	if (c != nullptr)	id = atoi(c);

	c = AST->getXMLValue(_property, 1, "r");
	if (c != nullptr)	bufferData.color.x = float(atof(c));
	c = AST->getXMLValue(_property, 1, "g");
	if (c != nullptr)	bufferData.color.y = float(atof(c));
	c = AST->getXMLValue(_property, 1, "b");
	if (c != nullptr)	bufferData.color.z = float(atof(c));

	c = AST->getXMLValue(_property, 1, "posX");
	if (c != nullptr)	bufferData.pos.x = float(atof(c));
	c = AST->getXMLValue(_property, 1, "posY");
	if (c != nullptr)	bufferData.pos.y = float(atof(c));
	c = AST->getXMLValue(_property, 1, "posZ");
	if (c != nullptr)	bufferData.pos.z = float(atof(c));

	c = AST->getXMLValue(_property, 1, "rotateCenterX");
	if (c != nullptr)	rotateCenter.x = float(atof(c));
	c = AST->getXMLValue(_property, 1, "rotateCenterY");
	if (c != nullptr)	rotateCenter.y = float(atof(c));
	c = AST->getXMLValue(_property, 1, "rotateCenterZ");
	if (c != nullptr)	rotateCenter.z = float(atof(c));

	c = AST->getXMLValue(_property, 1, "dirX");
	if (c != nullptr)	bufferData.dir.x = float(atof(c));
	c = AST->getXMLValue(_property, 1, "dirY");
	if (c != nullptr)	bufferData.dir.y = float(atof(c));
	c = AST->getXMLValue(_property, 1, "dirZ");
	if (c != nullptr)	bufferData.dir.z = float(atof(c));

	c = AST->getXMLValue(_property, 1, "speed");
	if (c != nullptr)	speed = atoi(c);

	c = AST->getXMLValue(_property, 1, "type");
	if (c != nullptr)	bufferData.param.x = float(atoi(c));
	c = AST->getXMLValue(_property, 1, "intensity");
	if (c != nullptr)	bufferData.param.y = float(atof(c));
	c = AST->getXMLValue(_property, 1, "coneAngle");
	if (c != nullptr)	bufferData.param.z = float(atof(c));

	c = AST->getXMLValue(_property, 1, "show");
	if (c != nullptr && atoi(c) == 1) bShow = true;

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
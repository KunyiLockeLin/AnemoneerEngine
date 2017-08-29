#include "qeheader.h"


void QeBillboard::setMatModel() {
	QeMatrix4x4f mat;

	size.x = 10;
	size.y = 10;
	size.z = 10;
	mat *= MATH->translate(pos);
	mat *= MATH->scale(size);

	mvp.model = mat;
}
/*
void QeBillBoard::setMatModelBillBoardFace() {
	modelData->vertices[0].normal;
	for (int i = 0; i<VP->currentNum; ++i) {

	}
}
void QeBillBoard::setMatModelBillBoardFaceAndPosition() {}
void QeBillBoard::setMatModelBillBoardFaceAndSize() {}
*/

void QeBillboard::update(float time) { updateUniformBuffer(); }
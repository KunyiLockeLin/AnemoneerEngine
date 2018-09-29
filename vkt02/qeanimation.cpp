#include "qeheader.h"

void QeAnimation::initialize(QeAssetXML* _property, QeObject* _owner) {
	QeModel::initialize(_property, _owner);

	AST->getXMLiValue((int*)&actionState, initProperty, 1, "actionState");
	AST->getXMLfValue(&actionSpeed, initProperty, 1, "actionSpeed");
	AST->getXMLiValue(&currentActionID, initProperty, 1, "actionID");
	AST->getXMLiValue((int*)&actionType, initProperty, 1, "actionType");

	graphicsShader.geom = nullptr;
	graphicsShader.tesc = nullptr;
	graphicsShader.tese = nullptr;
	graphicsShader.vert = nullptr;
	graphicsShader.frag = nullptr;
	shaderKey = "animation";

	AST->setGraphicsShader(graphicsShader, nullptr, shaderKey);
	currentActionFrame=0;
	currentActionTime=0;
}

void QeAnimation::update1() {
	updateAction();
	QeModel::update1();
}

bool QeAnimation::setAction(unsigned int actionID, QeActionType type) {
	if (actionID <= modelData->animationNum) {
		currentActionID = actionID;
		actionType = type;
		return true;
	}
	return false;
}

void QeAnimation::actionPlay() {

	currentActionFrame = modelData->animationStartFrames[currentActionID];
	currentActionTime = modelData->jointsAnimation[0].rotationInput[currentActionFrame];
	actionState = eActionStatePlay;
}

void QeAnimation::actionPause() { actionState = eActionStatePause; }
void QeAnimation::actionStop() { actionState = eActionStateStop; }

void QeAnimation::updateAction() {
	if (!modelData || !modelData->rootJoint || (actionState != eActionStatePlay && QE->deltaTime)) return;

	float previousActionFrameTime = modelData->jointsAnimation[0].rotationInput[currentActionFrame];
	float nextActionFrameTime = modelData->jointsAnimation[0].rotationInput[currentActionFrame + 1];
	bool bFinalFrame = false;
	if ((currentActionFrame + 1) == modelData->animationEndFrames[currentActionID]) bFinalFrame = true;

	float progessive = MATH->clamp((currentActionTime - previousActionFrameTime) / (nextActionFrameTime - previousActionFrameTime), 0.f, 1.f);

	QeVector3f previousTranslation, nextTranslation, currentTranslation;
	QeVector4f previousRotation, nextRotation, currentRotation;
	QeVector3f currentScale = { 1,1,1 };

	size_t size = modelData->jointsAnimation.size();

	for (size_t i = 0; i<size; ++i) {
		previousTranslation = modelData->jointsAnimation[i].translationOutput[currentActionFrame];
		nextTranslation = modelData->jointsAnimation[i].translationOutput[currentActionFrame + 1];
		currentTranslation = MATH->interpolatePos(previousTranslation, nextTranslation, progessive);

		previousRotation = modelData->jointsAnimation[i].rotationOutput[currentActionFrame];
		nextRotation = modelData->jointsAnimation[i].rotationOutput[currentActionFrame + 1];
		currentRotation = MATH->interpolateDir(previousRotation, nextRotation, progessive);

		jointTransforms[i] = MATH->transform(currentTranslation, currentRotation, currentScale);
	}

	QeMatrix4x4f transform;
	setChildrenJointTransform(*modelData->rootJoint, transform);

	currentActionTime += QE->deltaTime * actionSpeed;
	if (currentActionTime > nextActionFrameTime) {

		if (bFinalFrame) {
			if (actionType == eActionTypeOnce) actionStop();
			else {
				if (actionType == eActionTypeNext) {
					++currentActionID;
					if (currentActionID >= modelData->animationNum) currentActionID -= modelData->animationNum;
				}
				actionPlay();
			}
		}
		else	++currentActionFrame;
	}
}

void QeAnimation::setChildrenJointTransform(QeDataJoint& joint, QeMatrix4x4f &parentTransform) {

	size_t size = modelData->jointsAnimation.size();
	QeVector3f scale(1, 1, 1);

	for (size_t i = 0; i<size; ++i) {
		if (modelData->jointsAnimation[i].id == joint.id) {

			jointTransforms[i] = parentTransform * jointTransforms[i];

			size_t size1 = modelData->jointsAnimation[i].children.size();
			for (size_t j = 0; j<size1; ++j)
				setChildrenJointTransform(*modelData->jointsAnimation[i].children[j], jointTransforms[i]);

			bufferData.joints[i] = jointTransforms[i] * modelData->jointsAnimation[i].inverseBindMatrix;
			break;
		}
	}
}

QeMatrix4x4f QeAnimation::getBoneTransfrom(const char* boneName) {

	if (!boneName || !strlen(boneName))	return bufferData.model;

	size_t size = modelData->jointsAnimation.size();
	if (!size) return bufferData.model;

	for (size_t i = 0; i<size; ++i) {
		if (!strcmp(boneName, modelData->jointsAnimation[i].name))
			return bufferData.model*jointTransforms[i];
			//return bufferData.model*bufferData.joints[i];
	}
	return bufferData.model;
}
#include "header.h"

void QeAnimation::initialize(AeXMLNode *_property, QeObject *_owner) {
    COMPONENT_INITIALIZE_PARENT(QeModel)

    graphicsShader.geom = nullptr;
    graphicsShader.tesc = nullptr;
    graphicsShader.tese = nullptr;
    graphicsShader.vert = nullptr;
    graphicsShader.frag = nullptr;
    shaderKey = "animation";

    G_AST->setGraphicsShader(graphicsShader, nullptr, shaderKey);
    currentActionFrame = 0;
    currentActionTime = 0;
}

void QeAnimation::updatePreRender() {
    updateAction();
    QeModel::updatePreRender();
}

bool QeAnimation::setAction(unsigned int actionID, AE_ACTION_PLAY_TYPE type) {
    if (actionID <= modelData->animationNum) {
        component_data.actionID = actionID;
        component_data.actionPlayType = type;
        return true;
    }
    return false;
}

void QeAnimation::actionPlay() {
    currentActionFrame = modelData->animationStartFrames[component_data.actionID];
    currentActionTime = modelData->jointsAnimation[0].rotationInput[currentActionFrame];
    component_data.actionState = eACTION_Play;
}

void QeAnimation::actionPause() { component_data.actionState = eACTION_Pause; }
void QeAnimation::actionStop() { component_data.actionState = eACTION_Stop; }

void QeAnimation::updateAction() {
    if (!modelData || !modelData->rootJoint || (component_data.actionState != eACTION_Play && ENGINE->deltaTime)) return;

    float previousActionFrameTime = modelData->jointsAnimation[0].rotationInput[currentActionFrame];
    float nextActionFrameTime = modelData->jointsAnimation[0].rotationInput[currentActionFrame + 1];
    bool bFinalFrame = false;
    if ((currentActionFrame + 1) == modelData->animationEndFrames[component_data.actionID]) bFinalFrame = true;

    float progessive =
        MATH->clamp((currentActionTime - previousActionFrameTime) / (nextActionFrameTime - previousActionFrameTime), 0.f, 1.f);

    AeVector<float, 3> previousTranslation, nextTranslation, currentTranslation;
    AeVector<float, 4 > previousRotation, nextRotation, currentRotation;
    AeVector<float, 3> currentScale = {1, 1, 1};

    size_t size = modelData->jointsAnimation.size();

    for (size_t i = 0; i < size; ++i) {
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

    currentActionTime += ENGINE->deltaTime * component_data.actionSpeed;
    if (currentActionTime > nextActionFrameTime) {
        if (bFinalFrame) {
            if (component_data.actionPlayType == eACTION_PLAY_Once)
                actionStop();
            else {
                if (component_data.actionPlayType == eACTION_PLAY_Next) {
                    ++component_data.actionID;
                    if (component_data.actionID >= modelData->animationNum) component_data.actionID -= modelData->animationNum;
                }
                actionPlay();
            }
        } else
            ++currentActionFrame;
    }
}

void QeAnimation::setChildrenJointTransform(QeDataJoint &joint, QeMatrix4x4f &parentTransform) {
    size_t size = modelData->jointsAnimation.size();
    AeVector<float,3> scale({1, 1, 1});

    for (size_t i = 0; i < size; ++i) {
        if (modelData->jointsAnimation[i].id == joint.id) {
            jointTransforms[i] = parentTransform * jointTransforms[i];

            size_t size1 = modelData->jointsAnimation[i].children.size();
            for (size_t j = 0; j < size1; ++j)
                setChildrenJointTransform(*modelData->jointsAnimation[i].children[j], jointTransforms[i]);

            bufferData.joints[i] = jointTransforms[i] * modelData->jointsAnimation[i].inverseBindMatrix;
            break;
        }
    }
}

QeMatrix4x4f QeAnimation::getBoneTransfrom(const char *boneName) {
    if (!boneName || !strlen(boneName)) return bufferData.model;

    size_t size = modelData->jointsAnimation.size();
    if (!size) return bufferData.model;

    for (size_t i = 0; i < size; ++i) {
        if (!strcmp(boneName, modelData->jointsAnimation[i].name)) return bufferData.model * jointTransforms[i];
        // return bufferData.model*bufferData.joints[i];
    }
    return bufferData.model;
}

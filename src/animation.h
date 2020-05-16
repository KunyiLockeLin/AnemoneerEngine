#pragma once
#include "header.h"

class QeAnimation : public QeModel {
   public:
    COMPONENT_CLASS_DECLARE_PARENT(QeAnimation, QeModel, AeGameObjectComponentAnimationData)

    QeMatrix4x4f jointTransforms[MAX_JOINT_NUM];
    int currentActionFrame;
    float currentActionTime;

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);
    virtual void updatePreRender();

    bool setAction(unsigned int actionID, AE_ACTION_PLAY_TYPE playType);
    void actionPlay();
    void actionPause();
    void actionStop();
    void updateAction();
    void setChildrenJointTransform(QeDataJoint &joint, QeMatrix4x4f &parentTransform);
    QeMatrix4x4f getBoneTransfrom(const char *boneName);
};

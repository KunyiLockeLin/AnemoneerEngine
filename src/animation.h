#pragma once
#include "header.h"

class QeAnimation : public QeModel {
   public:
    QeAnimation(AeObjectManagerKey &_key) : QeModel(_key) {}
    ~QeAnimation() {}

    QeActionState actionState;
    QeActionType actionType;
    QeMatrix4x4f jointTransforms[MAX_JOINT_NUM];

    int currentActionID;
    int currentActionFrame;
    float currentActionTime;
    float actionSpeed;

    virtual void initialize(QeAssetXML *_property, QeObject *_owner);
    virtual void update1();

    bool setAction(unsigned int actionID, QeActionType playType);
    void actionPlay();
    void actionPause();
    void actionStop();
    void updateAction();
    void setChildrenJointTransform(QeDataJoint &joint, QeMatrix4x4f &parentTransform);
    QeMatrix4x4f getBoneTransfrom(const char *boneName);
};

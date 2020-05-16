#pragma once
#include "header.h"

class QePlane : public QeModel {
   public:
    COMPONENT_CLASS_DECLARE_PARENT(QePlane, QeModel, AeGameObjectComponentPlaneData)

    // face: (0,0,1)
    bool bUpdateTargetCameraOID;

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);
    virtual void clear();
    virtual void updatePreRedner();
    virtual QeDataDescriptorSetModel createDescriptorSetModel();
};

#pragma once

#include "header.h"

struct QeDataCamera {
    QeVector4f pos_rayTracingDepth;
    QeVector4f horizontal_aspect;
    QeVector4f vertical_lensRadius;
    QeVector4f lowerLeftCorner;
    QeMatrix4x4f view;
    QeMatrix4x4f projection;
};

class QeCamera : public QeComponent {
   public:
    QeCamera(AeObjectManagerKey &_key) : QeComponent(_key) {}
    ~QeCamera() {}

    QeRenderType renderType;
    int lookAtTransformOID;
    VkExtent2D renderSize;

    QeVector3f up;
    float aperture, fov, fnear, ffar, speed;

    int cullingDistance;
    QeVector2i lastMousePos;
    int postProcessingOID;
    bool bUpdatePostProcessingOID;
    QeDataCamera bufferData;

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);

    virtual void update1();

    virtual void reset();
    void setLookAtTransformOID(int _lookAtTransformOID);
    void rotateTarget(QeVector3f _addRotate);
    void rotateTargetByMouse(QeVector2i mousePos);
    void setMousePos(QeVector2i mousePos);
    void zoomInOut(QeVector2i mousePos);
    void move(QeVector3f _dir, bool bMoveCenter = true);
    bool isRaytracing();
    QeVector3f face();
    QeVector3f lookAt();
};

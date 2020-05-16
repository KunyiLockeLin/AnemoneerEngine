#pragma once

#include "header.h"

struct QeDataCamera {
    AeVector<float, 4> pos_rayTracingDepth;
    AeVector<float, 4> horizontal_aspect;
    AeVector<float, 4> vertical_lensRadius;
    AeVector<float, 4> lowerLeftCorner;
    QeMatrix4x4f view;
    QeMatrix4x4f projection;
};

class QeCamera : public QeComponent {
   public:
    COMPONENT_CLASS_DECLARE(QeCamera, AeGameObjectComponentCameraData)

    AeVector<int, 2> lastMousePos;
    bool bUpdatePostProcessingOID;
    QeDataCamera bufferData;

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);
    virtual void updatePreRender();

    virtual void reset();
    void setLookAtTransformOID(int _lookAtTransformOID);
    void rotateTarget(AeVector<float, 3> _addRotate);
    void rotateTargetByMouse(AeVector<int, 2> mousePos);
    void setMousePos(AeVector<int, 2> mousePos);
    void zoomInOut(AeVector<int, 2>  mousePos);
    void move(AeVector<float, 3> _dir, bool bMoveCenter = true);
    bool isRaytracing();
    AeVector<float, 3> face();
    AeVector<float, 3> lookAt();
};

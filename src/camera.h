#pragma once

#include "header.h"

struct QeDataCamera {
    AeArray<float, 4> pos_rayTracingDepth;
    AeArray<float, 4> horizontal_aspect;
    AeArray<float, 4> vertical_lensRadius;
    AeArray<float, 4> lowerLeftCorner;
    QeMatrix4x4f view;
    QeMatrix4x4f projection;
};

class QeCamera : public QeComponent {
   public:
    COMPONENT_CLASS_DECLARE(Camera)

    AeArray<int, 2> lastMousePos;
    bool bUpdatePostProcessingOID;
    QeDataCamera bufferData;

    virtual void updatePreRender();

    virtual void reset();
    void setLookAtTransformOID(int _lookAtTransformOID);
    void rotateTarget(AeArray<float, 3> _addRotate);
    void rotateTargetByMouse(AeArray<int, 2> mousePos);
    void setMousePos(AeArray<int, 2> mousePos);
    void zoomInOut(AeArray<int, 2>  mousePos);
    void move(AeArray<float, 3> _dir, bool bMoveCenter = true);
    bool isRaytracing();
    AeArray<float, 3> face();
    AeArray<float, 3> lookAt();
};

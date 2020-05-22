#pragma once
#include "header.h"

class QeAxis : public QeLine {
   public:
    COMPONENT_CLASS_DECLARE_PARENT(Axis, Line)

    virtual void updatePreRender();
};

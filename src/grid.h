#pragma once
#include "header.h"

class QeGrid : public QeLine {
   public:
    COMPONENT_CLASS_DECLARE_PARENT(Grid, Line)

    virtual void updatePreRender();
};

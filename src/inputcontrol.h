#pragma once
#include "header.h"

class QeInputControl : public QeComponent {
   public:
    COMPONENT_CLASS_DECLARE(InputControl)

    virtual void clear();

    virtual void updateInput();
};

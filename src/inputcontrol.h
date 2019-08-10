#pragma once
#include "component.h"

class QeInputControl : public QeComponent {
   public:
    QeInputControl(QeSceneManagerKey &_key) : QeComponent(_key) {}
    ~QeInputControl() {}

    virtual void initialize(QeAssetXML *_property, QeObject *_owner);
    virtual void clear();

    virtual void updateInput();
};

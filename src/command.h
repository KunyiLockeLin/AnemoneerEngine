#pragma once
#include "header.h"

class QeCommand {
   public:
    QeCommand(QeGlobalKey &_key) {}
    ~QeCommand() {}

    void inputCommand(std::string &command);
};

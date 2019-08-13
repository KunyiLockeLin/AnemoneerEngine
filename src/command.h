#pragma once
#include "header.h"

class QeCommand {
   public:
    QeCommand(AeGlobalKey &_key) {}
    ~QeCommand() {}

    void inputCommand(std::string &command);
};

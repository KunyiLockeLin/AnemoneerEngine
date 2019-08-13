#pragma once

#include "header.h"

class QeLog {
   public:
    QeLog(AeGlobalKey &_key) {}
    ~QeLog();

    std::ofstream ofile;

    std::string stack(int from, int to);
    void print(std::string &msg, bool bShowStack = false, int stackLevel = 4);

    bool isOutput();
};

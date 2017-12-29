#include "qeheader.h"


QeGlobal::QeGlobal():engine( new QueenEngine(key) ), window(new QeWindow(key)), viewport(new QeViewport(key)), vulkan(new QeVulkan(key)), math( new QeMath(key) ), asset( new QeAsset(key) ), objMgr(new QeObjectManger(key)), encode(new QeEncode(key)) {}

QeGlobal& QeGlobal::getInstance() { static QeGlobal _s; return _s; }
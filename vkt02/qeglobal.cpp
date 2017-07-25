#include "qeglobal.h"


QeGlobal::QeGlobal():engine( new QueenEngine(key) ), math( new QeMath(key) ), asset( new QeAsset(key) ), objMgr(new QeObjectManger(key)) {}

QeGlobal& QeGlobal::getInstance() { static QeGlobal _s; return _s; }
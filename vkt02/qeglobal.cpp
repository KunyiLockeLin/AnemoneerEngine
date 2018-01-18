#include "qeheader.h"


QeGlobal::QeGlobal():engine( new QueenEngine(key) ), window(new QeWindow(key)), viewport(new QeViewport(key)), 
vulkan(new QeVulkan(key)), math( new QeMath(key) ), asset( new QeAsset(key) ), objMgr(new QeObjectManger(key)), 
encode(new QeEncode(key)) {}

QeGlobal::~QeGlobal() {
	if (math != nullptr) {
		delete math;
		math = nullptr;
	}
	if (encode != nullptr) {
		delete encode;
		encode = nullptr;
	}
	if (viewport != nullptr) {
		delete viewport;
		viewport = nullptr;
	}
	if (asset != nullptr) {
		delete asset;
		asset = nullptr;
	}
	if (objMgr != nullptr) {
		delete objMgr;
		objMgr = nullptr;
	}
	if (vulkan != nullptr) {
		delete vulkan;
		vulkan = nullptr;
	}
	if (window != nullptr) {
		delete window;
		window = nullptr;
	}
	if (engine != nullptr) {
		delete engine;
		engine = nullptr;
	}
}

QeGlobal& QeGlobal::getInstance() { static QeGlobal _s; return _s; }
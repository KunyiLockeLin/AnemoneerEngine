#include "qeheader.h"


QeGlobal::QeGlobal() { init(); }

void QeGlobal::init() {
	if (engine == nullptr)		engine = new QueenEngine(key);
	if (window == nullptr)		window = new QeWindow(key);
	if (viewport == nullptr)	viewport = new QeViewport(key);
	if (vulkan == nullptr)		vulkan = new QeVulkan(key);
	if (math == nullptr)		math = new QeMath(key);
	if (asset == nullptr)		asset = new QeAsset(key);
	if (objMgr == nullptr)		objMgr = new QeObjectManger(key);
	if (encode == nullptr)		encode = new QeEncode(key);
}

QeGlobal::~QeGlobal() {
	
	cleanup();
	if (vulkan != nullptr) {
		delete vulkan;
		vulkan = nullptr;
	}
	if (window != nullptr) {
		delete window;
		window = nullptr;
	}
}

void QeGlobal::cleanup() {

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
	if (engine != nullptr) {
		delete engine;
		engine = nullptr;
	}
}

QeGlobal& QeGlobal::getInstance() { static QeGlobal _s; return _s; }

void QeGlobal::restart() { cleanup(); init(); WIN->resize(); }
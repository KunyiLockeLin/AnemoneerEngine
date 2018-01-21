#include "qeheader.h"


/*namespace  QEString {
	std::string operator+(std::string const &a, const int &b){
		std::ostringstream oss;
		oss << a << b;
		return oss.str();
	}
	std::string operator+(std::string const &a, const size_t &b){
		std::ostringstream oss;
		oss << a << b;
		return oss.str();
	}
	std::string operator+(std::string const &a, const float &b){
		std::ostringstream oss;
		oss << a << b;
		return oss.str();
	}
	std::string operator+(std::string const &a, const double &b){
		std::ostringstream oss;
		oss << a << b;
		return oss.str();
	}
	std::string operator+(std::string const &a, const char *b){
		std::ostringstream oss;
		oss << a << b;
		return oss.str();
	}
	std::string operator+=(std::string const &a, const char *b){
		std::ostringstream oss;
		oss << a << b;
		return oss.str();
	}
	std::string operator+=(std::string const &a, const int &b){
		std::ostringstream oss;
		oss << a << b;
		return oss.str();
	}
	std::string operator+=(std::string const &a, const size_t &b){
		std::ostringstream oss;
		oss << a << b;
		return oss.str();
	}
	std::string operator+=(std::string const &a, const float &b){
		std::ostringstream oss;
		oss << a << b;
		return oss.str();
	}
	std::string operator+=(std::string const &a, const double &b){
		std::ostringstream oss;
		oss << a << b;
		return oss.str();
	}
};*/

QeGlobal::QeGlobal() { init(); }

void QeGlobal::init() {
	if (engine == nullptr)		engine = new QueenEngine(key);
	if (window == nullptr)		window = new QeWindow(key);
	if (viewport == nullptr)	viewport = new QeViewport(key);
	if (math == nullptr)		math = new QeMath(key);
	if (asset == nullptr)		asset = new QeAsset(key);
	if (objMgr == nullptr)		objMgr = new QeObjectManger(key);
	if (encode == nullptr)		encode = new QeEncode(key);
	if (vulkan == nullptr)		vulkan = new QeVulkan(key);
	if (log == nullptr)			log = new QeLog(key);
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
	if (log != nullptr) {
		delete log;
		log = nullptr;
	}
}

QeGlobal& QeGlobal::getInstance() { static QeGlobal _s; return _s; }

void QeGlobal::restart() { cleanup(); init(); WIN->resize(); }
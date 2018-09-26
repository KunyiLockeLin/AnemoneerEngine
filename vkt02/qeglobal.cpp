#include "qeheader.h"


namespace  QeLib {
	std::string toString(const int & i) {
		std::ostringstream oss;
		oss << i;
		return oss.str();
	}
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
};

QeGlobal::QeGlobal() {
	if (engine == nullptr)		engine = new QueenEngine(key);
	if (window == nullptr)		window = new QeWindow(key);
	if (scene == nullptr)		scene = new QeScene(key);
	if (graphics == nullptr)	graphics = new QeGraphics(key);
	if (math == nullptr)		math = new QeMath(key);
	if (asset == nullptr)		asset = new QeAsset(key);
	if (objMgr == nullptr)		objMgr = new QeObjectManger(key);
	if (encode == nullptr)		encode = new QeEncode(key);
	if (vulkan == nullptr)		vulkan = new QeVulkan(key);
	if (log == nullptr)			log = new QeLog(key);
	if (command == nullptr)		command = new QeCommand(key);
}

QeGlobal::~QeGlobal() {	
	if (math != nullptr) {
		delete math;
		math = nullptr;
	}
	if (encode != nullptr) {
		delete encode;
		encode = nullptr;
	}
	if (graphics != nullptr) {
		delete graphics;
		graphics = nullptr;
	}
	if (asset != nullptr) {
		delete asset;
		asset = nullptr;
	}
	if (objMgr != nullptr) {
		delete objMgr;
		objMgr = nullptr;
	}
	if (log != nullptr) {
		delete log;
		log = nullptr;
	}
	if (command != nullptr) {
		delete command;
		command = nullptr;
	}
	if (vulkan != nullptr) {
		delete vulkan;
		vulkan = nullptr;
	}
	if (scene != nullptr) {
		delete scene;
		scene = nullptr;
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
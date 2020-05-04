#include "header.h"
#include <sstream>

namespace QeLib {
std::string toString(const int &i) {
    std::ostringstream oss;
    oss << i;
    return oss.str();
}
std::string operator+(std::string const &a, const int &b) {
    std::ostringstream oss;
    oss << a << b;
    return oss.str();
}
std::string operator+(std::string const &a, const size_t &b) {
    std::ostringstream oss;
    oss << a << b;
    return oss.str();
}
std::string operator+(std::string const &a, const float &b) {
    std::ostringstream oss;
    oss << a << b;
    return oss.str();
}
std::string operator+(std::string const &a, const double &b) {
    std::ostringstream oss;
    oss << a << b;
    return oss.str();
}
std::string operator+(std::string const &a, const char *b) {
    std::ostringstream oss;
    oss << a << b;
    return oss.str();
}
std::string operator+=(std::string const &a, const char *b) {
    std::ostringstream oss;
    oss << a << b;
    return oss.str();
}
std::string operator+=(std::string const &a, const int &b) {
    std::ostringstream oss;
    oss << a << b;
    return oss.str();
}
std::string operator+=(std::string const &a, const size_t &b) {
    std::ostringstream oss;
    oss << a << b;
    return oss.str();
}
std::string operator+=(std::string const &a, const float &b) {
    std::ostringstream oss;
    oss << a << b;
    return oss.str();
}
std::string operator+=(std::string const &a, const double &b) {
    std::ostringstream oss;
    oss << a << b;
    return oss.str();
}
};  // namespace QeLib

AeGlobal::AeGlobal() {
    if (engine == nullptr) engine = new AngryEngine(key);
    if (ui == nullptr) ui = new AeUI(key);
    if (objectmanager == nullptr) objectmanager = new AeObjectManager(key);
    if (graphics == nullptr) graphics = new QeGraphics(key);
    if (vulkan == nullptr) vulkan = new QeVulkan(key);
    if (command == nullptr) command = new QeCommand(key);
}

AeGlobal::~AeGlobal() {
    if (command != nullptr) {
        delete command;
        command = nullptr;
    }
    if (graphics != nullptr) {
        delete graphics;
        graphics = nullptr;
    }
    if (vulkan != nullptr) {
        delete vulkan;
        vulkan = nullptr;
    }
    if (scene != nullptr) {
        delete scene;
        scene = nullptr;
    }
    if (ui != nullptr) {
        delete ui;
        ui = nullptr;
    }
    if (engine != nullptr) {
        delete engine;
        engine = nullptr;
    }
}

AeGlobal &AeGlobal::getInstance() {
    static AeGlobal _s;
    return _s;
}

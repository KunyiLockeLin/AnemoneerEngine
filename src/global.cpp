#include "header.h"

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
    if (window == nullptr) window = new QeWindow(key);
    if (objectmanager == nullptr) objectmanager = new AeObjectManager(key);
    if (graphics == nullptr) graphics = new QeGraphics(key);
    if (math == nullptr) math = new QeMath(key);
    if (asset == nullptr) asset = new QeAsset(key);
    if (encode == nullptr) encode = new QeEncode(key);
    if (vulkan == nullptr) vulkan = new QeVulkan(key);
    if (log == nullptr) log = new QeLog(key);
    if (command == nullptr) command = new QeCommand(key);
}

QeAssetXML *AeGlobal::getConfigXML() {
    if (configXML) return configXML;
    return configXML = asset->getXML(asset->CONFIG_PATH);
}

AeGlobal::~AeGlobal() {
    if (math != nullptr) {
        delete math;
        math = nullptr;
    }
    if (encode != nullptr) {
        delete encode;
        encode = nullptr;
    }
    if (asset != nullptr) {
        delete asset;
        asset = nullptr;
    }
    if (log != nullptr) {
        delete log;
        log = nullptr;
    }
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
    if (window != nullptr) {
        delete window;
        window = nullptr;
    }
    if (engine != nullptr) {
        delete engine;
        engine = nullptr;
    }
    configXML = nullptr;
}

AeGlobal &AeGlobal::getInstance() {
    static AeGlobal _s;
    return _s;
}

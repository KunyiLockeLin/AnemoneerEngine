#include "common.h"
#include <ctime>
#include <windows.h>
#include <direct.h>
#include "dbghelp.h"
#include <sstream>
#include <iostream>
#include <cerrno>

namespace AeLib {
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
};  // namespace AeLib

AeFile::AeFile() : ofile(new std::ofstream()), output_path(new std::string()) {}
AeFile::~AeFile() {
    ofile->close();
    delete ofile;
    ofile = nullptr;
    delete output_path;
    output_path = nullptr;
}

bool AeFile::open(const char *output_path_) {
    *output_path = output_path_;
    std::vector<std::string> output_dirs = ENCODE->split<std::string>(output_path_, "\\");
    output_dirs.pop_back();
    if (output_dirs.size() > 0) {
        std::string output_dir = ENCODE->combine<std::string>(output_dirs, "\\");
        _mkdir(output_dir.c_str());
    }
    ofile->open(output_path_);
    if (ofile->fail()) {
        //std::cout << "open failure as expected: " << strerror(errno) << '\n';
        return false;
    }
    return true;
}

bool AeFile::isOpen() { return ofile->is_open(); }

bool AeFile::addNewLine(const char *s) {
    if (!isOpen()) return false;
    *ofile << s << std::endl;
    return true;
}

void AeFile::close() { ofile->close(); }

AeLog::AeLog() : listeners(new std::vector<AeLogListener *>()), file(nullptr) {}

AeLog::~AeLog() {
    if (file) {
        file->close();
        delete file;
        file = nullptr;
    }
    listeners->clear();
    delete listeners;
    listeners = nullptr;
}

// bool QeLog::isOutput() { return CONFIG->getXMLValuei("setting.environment.outputLog"); }
bool AeLog::isOutput() { return (file && file->isOpen()) ? true : false; }

void AeLog::switchOutput(bool turn_on, const char *output_path) {
    if (turn_on) {
        if (!file) {
            file = new AeFile();
        }
        if (file->isOpen()) {
            file->close();
        }
        time_t rawtime;
        struct tm timeinfo;
        char buffer[128];

        time(&rawtime);
        localtime_s(&timeinfo, &rawtime);

        strftime(buffer, sizeof(buffer), "%y%m%d%H%M%S", &timeinfo);
        std::string outputPath = output_path;
        outputPath += buffer;
        outputPath += ".txt";
        file->open(outputPath.c_str());
    } else {
        file->close();
    }
}

void AeLog::addListener(AeLogListener &listener) { listeners->emplace_back(&listener); }
void AeLog::removeListener(AeLogListener &listener) { eraseElementFromVector(*listeners, &listener); }

std::string AeLog::stack(int from, int to) {
    std::string ret = "";

    void **backTrace = new void *[to];

    const USHORT nFrame = CaptureStackBackTrace(from, to, backTrace, nullptr);

    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);

    HANDLE hProcess = GetCurrentProcess();
    SymInitialize(hProcess, NULL, TRUE);

    for (USHORT iFrame = 0; iFrame < nFrame; ++iFrame) {
        DWORD64 displacement64;
        char symbol_buffer[sizeof(SYMBOL_INFO) + 256];
        SYMBOL_INFO *symbol = reinterpret_cast<SYMBOL_INFO *>(symbol_buffer);
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = 255;
        SymFromAddr(hProcess, (DWORD64)backTrace[iFrame], &displacement64, symbol);

        DWORD displacement;
        IMAGEHLP_LINE64 line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        SymGetLineFromAddr64(GetCurrentProcess(), (DWORD64)backTrace[iFrame], &displacement, &line);

        char s[512];

        sprintf_s(s, "\n%1d %s %d", iFrame, symbol->Name, line.LineNumber);
        ret.append(s);
    }
    SymCleanup(hProcess);

    delete[] backTrace;

    return ret;
}

void AeLog::print(std::string &msg, bool bShowStack, int stackLevel) {
    if (this == nullptr) return;

    time_t rawtime;
    struct tm timeinfo;
    char buffer[128];

    time(&rawtime);
    localtime_s(&timeinfo, &rawtime);

    strftime(buffer, sizeof(buffer), "%y%m%d%H%M%S ", &timeinfo);
    std::string s = buffer;
    s += msg;

    if (bShowStack) s += stack(2, stackLevel);

    if (file && file->isOpen()) {
        // ofile.seekp(ofile.beg);
        file->addNewLine(s.c_str());
        // ofile.flush();
    }
    std::cout << s << std::endl;
    for (auto *listener : *listeners) {
        listener->updateLog(s.c_str());
    }
}

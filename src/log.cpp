#include "header.h"

QeLog::~QeLog() {
    if (ofile.is_open()) {
        ofile.close();
    }
}

bool QeLog::isOutput() { return CONFIG->getXMLValuei("setting.environment.outputLog"); }

std::string QeLog::stack(int from, int to) {
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

void QeLog::print(std::string &msg, bool bShowStack, int stackLevel) {
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

    WIN->Log(s);
    if (isOutput()) {
        if (!ofile.is_open()) {
            time_t rawtime;
            struct tm timeinfo;
            char buffer[128];

            time(&rawtime);
            localtime_s(&timeinfo, &rawtime);

            strftime(buffer, sizeof(buffer), "%y%m%d%H%M%S", &timeinfo);
            std::string outputPath = CONFIG->getXMLValue("setting.path.log");
            _mkdir(outputPath.c_str());
            outputPath += "log";
            outputPath += buffer;
            outputPath += ".txt";

            ofile.open(outputPath);
        }
        // ofile.seekp(ofile.beg);
        ofile << s << std::endl;
        // ofile.flush();
    }
}

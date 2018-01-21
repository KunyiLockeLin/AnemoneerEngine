#pragma once

#include "qeheader.h"

enum QeDebugMode {
	eModeNoDebug,
	eModeConsole,
	eModeOutput,
};

class QeLog
{
public:
	QeLog(QeGlobalKey& _key) {}
	~QeLog() {}

	QeDebugMode mode = eModeNoDebug;
	std::string outputPath;
	void init();
	void print(std::string& msg);
	bool isDebug();
	bool isConsole();
};


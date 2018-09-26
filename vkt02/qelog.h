#pragma once

#include "qeheader.h"

enum QeDebugMode {
	eModeNoDebug,
	eModeConsole,
	eModeOutput,
	eModeConsoleOutput,
};

class QeLog
{
public:
	QeLog(QeGlobalKey& _key) {}
	~QeLog();

	std::ofstream ofile;

	std::string stack(int from, int to);
	void print(std::string& msg, bool bShowStack=false, int stackLevel=4);
	
	QeDebugMode mode();
	bool isDebug();
	bool isConsole();
	bool isOutput();
};
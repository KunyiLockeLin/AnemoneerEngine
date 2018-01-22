#pragma once
#include "qeheader.h"


class QeCommand
{
public:
	QeCommand(QeGlobalKey& _key) {}
	~QeCommand() {}

	void inputCommand( std::string& command);
};


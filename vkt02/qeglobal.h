#pragma once

#include "qeheader.h"


class QeGlobalKey
{
	friend class QeGlobal;

private:
	QeGlobalKey() {}
};

class QeGlobal
{
private:
	QeGlobal();
	QeGlobalKey	key;

public:
	~QeGlobal() {}
	static QeGlobal&	getInstance();
	QueenEngine*		engine;
	QeMath*				math;
	QeAsset*			asset;
};

#define GLB		QeGlobal::getInstance()
#define QE		GLB.engine
#define MATH	GLB.math
#define AST		GLB.asset
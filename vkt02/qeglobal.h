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
	QeVulkan*			vulkan;
	QeWindow*			window;
	QeViewport*			viewport;
	QeMath*				math;
	QeAsset*			asset;
	QeObjectManger*		objMgr;
	QeEncode*			encode;
};

#define GLB		QeGlobal::getInstance()
#define QE		GLB.engine
#define VLK		GLB.vulkan
#define WIN		GLB.window
#define VP		GLB.viewport
#define MATH	GLB.math
#define AST		GLB.asset
#define OBJMGR	GLB.objMgr
#define ENCODE	GLB.encode
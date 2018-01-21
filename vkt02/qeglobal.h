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
	void init();
	void cleanup();
public:
	~QeGlobal();
	void restart();
	static QeGlobal&	getInstance();
	QueenEngine*		engine = nullptr;
	QeVulkan*			vulkan = nullptr;
	QeWindow*			window = nullptr;
	QeViewport*			viewport = nullptr;
	QeMath*				math = nullptr;
	QeAsset*			asset = nullptr;
	QeObjectManger*		objMgr = nullptr;
	QeEncode*			encode = nullptr;
};

#define GLB		QeGlobal::getInstance()
#define QE		GLB.engine
#define VK		GLB.vulkan
#define WIN		GLB.window
#define VP		GLB.viewport
#define MATH	GLB.math
#define AST		GLB.asset
#define OBJMGR	GLB.objMgr
#define ENCODE	GLB.encode
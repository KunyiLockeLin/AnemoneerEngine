#pragma once

#include "qeheader.h"


namespace  QeString {

	std::string toString(const int &i);
	std::string operator+(std::string const &a, const int &b);
	std::string operator+(std::string const &a, const size_t &b);
	std::string operator+(std::string const &a, const float &b);
	std::string operator+(std::string const &a, const double &b);
	std::string operator+(std::string const &a, const char *b);
	std::string operator+=(std::string const &a, const int &b);
	std::string operator+=(std::string const &a, const size_t &b);
	std::string operator+=(std::string const &a, const float &b);
	std::string operator+=(std::string const &a, const double &b);
	std::string operator+=(std::string const &a, const char *b);
};


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
	//bool libGLFW;
	//bool libGLM;
	//bool libGLI;
	static QeGlobal&	getInstance();
	QueenEngine*		engine = nullptr;
	QeVulkan*			vulkan = nullptr;
	QeWindow*			window = nullptr;
	QeGraphics*			graphics = nullptr;
	QeMath*				math = nullptr;
	QeAsset*			asset = nullptr;
	QeObjectManger*		objMgr = nullptr;
	QeEncode*			encode = nullptr;
	QeLog*				log = nullptr;
	QeCommand*			command = nullptr;
};

#define GLB		QeGlobal::getInstance()
#define QE		GLB.engine
#define VK		GLB.vulkan
#define WIN		GLB.window
#define VP		GLB.graphics
#define ACT		QE->activity
#define MATH	GLB.math
#define AST		GLB.asset
#define OBJMGR	GLB.objMgr
#define ENCODE	GLB.encode
#define DEBUG	GLB.log
#define LOG(msg) GLB.log->print( std::string("")+msg )
#define CMD(msg) GLB.command->inputCommand( msg )
#pragma once

#include "qeheader.h"


namespace  QeLib {

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

	template <class T>
	int findElementFromVector(std::vector<T> & vec, T element) {
		std::vector<T>::iterator it = std::find(vec.begin(), vec.end(), element);
		if (it == vec.end()) return INDEX_NONE;
		return int(it - vec.begin());
	}

	template <class T>
	bool eraseElementFromVector(std::vector<T> & vec, T element) {
		int index = findElementFromVector(vec, element);
		if (index == INDEX_NONE) return false;
		vec.erase(vec.begin() + index);
		return true;
	}
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
	void initialize();
	void clear();
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
	QeScene*			scene = nullptr;
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
#define GRAP	GLB.graphics
#define SCENE	GLB.scene
#define MATH	GLB.math
#define AST		GLB.asset
#define OBJMGR	GLB.objMgr
#define ENCODE	GLB.encode
#define DEBUG	GLB.log
#define CMD(msg) GLB.command->inputCommand( msg )

#define LOG(msg)	GLB.log->print( std::string("")+msg )
#define STACK(msg)	GLB.log->print( std::string("")+msg, true)
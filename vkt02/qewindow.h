#pragma once

#include "qeheader.h"

class QeWindow
{
public:
	QeWindow(QeGlobalKey& _key) {}
	~QeWindow() {}

	void getWindowSize(int& width, int& height);
	void update(float time);
	HWND window;
	HINSTANCE windowInstance;
	bool bInit = false;
	void init();
	void resize();
	std::string getWindowTitle();

	TCHAR* convert(std::string _s); 
	void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
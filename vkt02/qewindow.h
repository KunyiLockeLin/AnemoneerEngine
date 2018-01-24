#pragma once

#include "qeheader.h"

class QeWindow
{
public:
	QeWindow(QeGlobalKey& _key) {}
	~QeWindow() {}

	HINSTANCE windowInstance;
	HWND window;
	HWND commandBox;
	WNDPROC DefEditProc;

	void getWindowSize(int& width, int& height);
	void update(float time);
	bool bInit = false;
	void init();
	void resize();
	std::string getWindowTitle();

	std::wstring chartowchar(std::string s);
	std::string wchartochar(std::wstring s);
	void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void sendCommand();
	void closeCommand();
	//bool isWindow();
};
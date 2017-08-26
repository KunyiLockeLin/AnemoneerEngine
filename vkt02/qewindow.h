#pragma once

#include "qeheader.h"

class QeWindow
{
public:
	QeWindow(QeGlobalKey& _key) {}
	~QeWindow() {}

	//int currentWidth = 0;
	//int currentHeight = 0;
	bool isWinodowShouldClose();
	void getWindowSize(int& width, int& height);
	void update(float time);
	HWND window;
	HINSTANCE windowInstance;
	VkSurfaceKHR surface;
	bool bClosed;
	void init();
	
	std::string getWindowTitle();
	void cleanup();

	TCHAR* convert(std::string _s); 
	void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
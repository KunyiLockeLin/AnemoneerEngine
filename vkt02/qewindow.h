#pragma once

#include "qeheader.h"

class QeWindow
{
public:
	QeWindow(QeGlobalKey& _key) {}
	~QeWindow() {}

	const int WIDTH = 800;
	const int HEIGHT = 600;

	bool isWinodowShouldClose();
	void getWindowSize(int& width, int& height);
	void update(float time);
	HWND window;
	HINSTANCE windowInstance;
	VkSurfaceKHR surface;
	bool bClosed;
	void init();
	void initSurface();
	std::string getWindowTitle();
	void cleanup();

	TCHAR* convert(std::string _s); 
	void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
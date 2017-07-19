#pragma once

#include "qeheader.h"

class QeWindow
{
public:
	QeWindow() {}
	~QeWindow() {}

	const int WIDTH = 800;
	const int HEIGHT = 600;

	float fpsTimer = 0.0f;
	uint32_t frameCounter = 0;
	uint32_t lastFPS = 0;
	float frameTimer = 1.0f;

	bool isWinodowShouldClose();
	void getWindowSize(int& width, int& height);
	void update();
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
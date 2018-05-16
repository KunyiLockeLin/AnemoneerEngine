#pragma once

#include "qeheader.h"

struct QeInputData {
	int			inputType;
	int			inputKey;
	QeVector2i	mousePos;
	std::string consoleCommandInput;
};

class QeWindow
{
public:
	QeWindow(QeGlobalKey& _key) {}
	~QeWindow() {}

	HINSTANCE windowInstance;
	HWND window;
	HWND commandBox;
	WNDPROC DefEditProc;
	QeInputData inputData;

	void getWindowSize(int& width, int& height);
	void updateRender(float time);
	void updateCompute(float time);
	bool bInit = false;
	void init();
	void resize();
	std::string getWindowTitle();

	std::wstring chartowchar(std::string s);
	std::string wchartochar(std::wstring s);
	void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void sendCommand();
	void closeCommand();
	void consoleInput();
	//bool isWindow();
};
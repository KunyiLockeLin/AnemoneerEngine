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
	std::vector<QeInputControl*> inputControls;

	void getWindowSize(int& width, int& height);
	void update1();
	void update2();
	bool bInit = false;
	void initialize();
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
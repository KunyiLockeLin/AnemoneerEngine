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
	HWND mainWindow, commandBox, editWindow, tabControlCategory, treeViewList, listViewDetail, listBoxLog;
	WNDPROC DefEditProc;

	int currentTab;
	QeInputData inputData;
	std::vector<QeInputControl*> inputControls;

	void getWindowSize(HWND & window, int& width, int& height);
	void update1();
	void update2();
	bool bInit = false;
	void initialize();
	void openMainWindow();
	void openEditWindow();
	void updateTab();
	void resize(HWND & window);
	std::string getWindowTitle();
	void Log(std::string _log);

	std::wstring chartowchar(std::string s);
	std::string wchartochar(std::wstring s);
	void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void sendCommand();
	void closeCommand();
	void consoleInput();
	//bool isWindow();
};
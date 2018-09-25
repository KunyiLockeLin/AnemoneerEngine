#pragma once

#include "qeheader.h"

struct QeInputData {
	int			inputType;
	int			inputKey;
	QeVector2i	mousePos;
	std::string consoleCommandInput;
};

enum QeUIType {
	eUIType_btnPause=1,
	eUIType_btnLoadAll=2,
	eUIType_btnSaveAll=3,
	eUIType_btnLoadScene=4,
	eUIType_btnSaveEID=5,
	eUIType_btnLoadEID=6,
	eUIType_btnSetCamera=7,
	eUIType_btnNewScene = 8,
	eUIType_btnNewObject = 9,
	eUIType_btnNewComponent = 10,
};

class QeWindow
{
public:
	QeWindow(QeGlobalKey& _key) {}
	~QeWindow() {}

	HINSTANCE windowInstance;
	HWND mainWindow, commandBox, editWindow, tabControlCategory, treeViewList, listViewDetail, listBoxLog, currentEditListView;
	WNDPROC DefEditProc;
	HWND btnPause, btnLoadAll, btnSaveAll, btnLoadScene, btnSaveEID, btnLoadEID, btnSetCamera, btnNewScene, btnNewObject, btnNewComponent;

	int currentTabIndex;
	QeAssetXML * currentTreeViewNode;
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
	void updateListView();
	void updateListViewItem();
	void resize(HWND & window);
	std::string getWindowTitle();
	void Log(std::string _log);
	void addToTreeView(QeAssetXML * node, HTREEITEM parent);

	std::wstring chartowchar(std::string s);
	std::string wchartochar(std::wstring s);
	void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void sendCommand();
	void closeCommand();
	void consoleInput();
	//bool isWindow();
};
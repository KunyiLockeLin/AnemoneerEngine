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
	HWND mainWindow, commandBox, editWindow, tabControlCategory, treeViewList, listViewDetail, listBoxLog, currentEditListView;
	WNDPROC DefEditProc;
	HWND btnPause, btnUpdateAll, btnLoadAll, btnSaveAll, btnLoadScene, btnSaveEID, btnLoadEID, btnCameraFocus, btnCameraControl, btnNewItem, btnDeleteItem;

	int currentTabIndex;
	int currentTreeViewNodeIndex;
	std::string currentEditListViewKey;
	std::vector<QeAssetXML*> currentTreeViewNodes;
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
	void resizeAll();
	void resize(HWND & window);
	std::string getWindowTitle();
	void Log(std::string _log);
	void addToTreeView(QeAssetXML * node, HTREEITEM parent);
	void adjustComponetData(QeAssetXML * node);
	void setTreeViewText(HTREEITEM hItem, QeAssetXML * node);

	std::wstring chartowchar(std::string s);
	std::string wchartochar(std::wstring s);
	void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void sendCommand();
	void closeCommand();
	void consoleInput();
	//bool isWindow();
};
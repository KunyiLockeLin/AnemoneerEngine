#pragma once

#include "header.h"

struct AeInputData {
    int inputType;
    int inputKey;
    AeArray<int, 2> mousePos;
    std::string consoleCommandInput;
};

class AeUI : public AeLogListener {
   public:
    AeUI(AeGlobalKey &_key) {}
    ~AeUI();

    HINSTANCE windowInstance;
    HWND mainWindow, commandBox;
    HWND editPanel, tabControlCategory, listViewDetail, currentEditListView;
    std::vector<HWND> treeViewLists;
    HWND logPanel, listBoxLog;
    HDC logHDC;
    LONG logMaxWidth;
    WNDPROC DefEditProc;
    HWND btnPause, btnUpdateAll, btnLoadAll, btnSaveAll, btnLoadScene, btnSaveEID, btnLoadEID, btnCameraFocus, btnCameraControl,
        btnNewItem, btnDeleteItem;

    int currentTabIndex;
    // int currentTreeViewNodeIndex;
    std::string currentEditListViewKey;
    bool bAddTreeView = false;
    // std::vector<AeXMLNode*> currentTreeViewNodes;
    AeXMLNode *currentTreeViewNode;
    AeInputData inputData;
    std::vector<QeInputControl *> inputControls;

    void getWindowSize(HWND &window, int &width, int &height);
    void update1();
    void update2();
    bool bInit = false;
    void initialize();
    void openMainWindow();
    void openEditPanel();
    void openLogPanel();
    void setAllTreeView();
    void updateTab();
    void updateListView();
    void updateListViewItem();
    void resizeAll();
    void resize(HWND &window);
    std::string getWindowTitle();
    void Log(std::string _log);
    void addToTreeView(AeXMLNode *node, HTREEITEM parent);
    void adjustComponetData(AeXMLNode *node);
    void setTreeViewText(HTREEITEM hItem, AeXMLNode *node);

    std::wstring chartowchar(std::string s);
    std::string wchartochar(std::wstring s);
    void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void sendCommand();
    void closeCommand();
    virtual void updateLog(const char *msg) { Log(msg);}
    // void consoleInput();
    // bool isWindow();
};

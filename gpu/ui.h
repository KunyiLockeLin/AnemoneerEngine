#ifndef AE_UI_H
#define AE_UI_H

// TODO: ui needs to be replaced with lib.ui when ilb.ui is completed.

//#include "src/header.h"
#include "common/common.h"
#include <windows.h>
#include <commctrl.h>

const char CONFIG_PATH[] = "data\\config.xml";
#define CONFIG COM_MGR.getXML(CONFIG_PATH)

class AeGlobalKey {
    friend class AeGlobal;

   private:
    AeGlobalKey() {}
};

struct AeInputData {
    int inputType;
    int inputKey;
    AeArray<int, 2> mousePos;
    std::string consoleCommandInput;
};

class QeInputControl {
   public:
    void updateInput() {} // TODO: Do nothing for temporay code.
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

    std::string window_title_{""};
    bool closed{false};

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

class AeGlobal {
   private:
    AeGlobal() {
        if (ui == nullptr) ui = new AeUI(key);
    }
    AeGlobalKey key;

   public:
    ~AeGlobal() {
        if (ui != nullptr) {
            delete ui;
            ui = nullptr;
        }
    }

    static AeGlobal &getInstance() {
        static AeGlobal _s;
        return _s;
    }

    AeUI *ui = nullptr;
};

#define GLB AeGlobal::getInstance()
#define UI GLB.ui

#endif  // AE_UI_H
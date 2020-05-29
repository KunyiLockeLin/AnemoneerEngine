#pragma once

#include <windows.h>
#include <commctrl.h>
#include "common/common.h"
#include <functional>
#include "generated/generated_config_struct_enum.h"

#define KEY_FSLASH 0x2F
#define KEY_A 0x41
#define KEY_C 0x43
#define KEY_D 0x44
#define KEY_E 0x45
#define KEY_Q 0x51
#define KEY_R 0x52
#define KEY_S 0x53
#define KEY_W 0x57
//#define KEY_W 229
#define KEY_L 0x4C
#define KEY_X 0x58
#define KEY_Z 0x5A

enum QeUIType {
    eUIType_btnPause = 1,
    eUIType_btnUpdateAll = 2,
    eUIType_btnLoadAll = 3,
    eUIType_btnSaveAll = 4,
    eUIType_btnLoadScene = 5,
    eUIType_btnSaveEID = 6,
    eUIType_btnLoadEID = 7,
    eUIType_btnCameraFocus = 8,
    eUIType_btnNewItem = 9,
    eUIType_btnDeleteItem = 10,
    eUIType_btnCameraControl = 11,
};

enum AeUIEventType {
    eUIEvent_Click = 1,
};

struct AeUIInputData {
    int inputType;
    int inputKey;
    AeArray<int, 2> mousePos;
    std::string consoleCommandInput;
};

class AeUIInputListener {
   public:
    virtual void updateInput(const AeUIInputData &intput) {}
};

class AeUILCommandistener {
   public:
    virtual void updateCommand(const std::string &command) {}
};

using UIEventRegisterFunction = std::function<void()>;

MANAGER_KEY_CLASS(UI);

class AeUIItemWin32 {};

class AeUISet {};

class AeUIWindow {
};

class AeUIComponent {
   public:
    AeBaseData data;
    AeUIItemWin32 item;
    void AddEvent(AeUIEventType type, UIEventRegisterFunction function) {}
    void RemoveEvent(AeUIEventType type);
};

class AeUIMgr : public AeLogListener {
    SINGLETON_CLASS(AeUIMgr);

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

    AeUIInputData inputData;
    std::vector<AeUIInputListener *> input_listeners;
    std::vector<AeUILCommandistener *> command_listeners;

    std::map<AeUIEventType, std::map<ID, UIEventRegisterFunction>> event_register;
    void AddEvent(AeUIEventType type, ID oid, UIEventRegisterFunction function);
    void RemoveEvent(AeUIEventType type, ID oid);

    int currentTabIndex;
    // int currentTreeViewNodeIndex;
    std::string currentEditListViewKey;
    bool bAddTreeView = false;
    // std::vector<AeXMLNode*> currentTreeViewNodes;
    AeXMLNode *currentTreeViewNode;

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
    virtual void updateLog(const char *msg) { Log(msg); }
    // void consoleInput();
    // bool isWindow();
};
#define UI_MGR AeUIMgr::getInstance()

const char CONFIG_PATH[] = "data\\config.xml";
#define CONFIG COM_MGR.getXML(CONFIG_PATH)

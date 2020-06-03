#pragma once

#include <windows.h>
#include <commctrl.h>
#include "common/common.h"
#include <functional>
#include "code_generator/generated_config_struct_enum.h"

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

class AeUISet : public AeUIComponentBase {
   public:
    AeUISet(AeUIManagerKey &_key) : AeUIComponentBase(_key) {}
    ~AeUISet() {}

    AeUIInputData inputData;
    std::vector<AeUIInputListener *> inputControls;
    std::vector<AeUIComponentListBox *> listboxs;
    AeArray<int, 2> screenSize;
    bool bResize = false;

    virtual void initialize(AeXMLNode *_property, AeUIWindow *_owner);
    virtual void clear();
    virtual void updatePreRender();

    void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void resize(HWND targetHandle);

    void Log(std::string &s);
    std::wstring chartowchar(std::string s);
    std::string wchartochar(std::wstring s);
};

class AeUIWindow : public AeUIComponentBase {
   public:
    AeUIWindow(AeUIManagerKey &_key) : AeUIComponentBase(_key) {}
    ~AeUIWindow() {}

    virtual void initialize(AeXMLNode *_property, AeUIComponentBase *_owner);
    virtual void clear();
    virtual void updatePreRender();
    virtual void updatePostRedner();

    void resize(HWND targetHandle);
};

class AeUIComponentBase;
class AeUIItemBase {
   public:
    AeUIItemBase(AeUIManagerKey &_key, AeUIComponentBase *_owner) : owner(_owner) {}
    ~AeUIItemBase() {}

    AeUIComponentBase *owner = nullptr;
    HINSTANCE win32Instance;
    HWND win32Handle;
    HDC win32DeviceContext;

    AeArray<int, 2> position, size;
    AE_ALIGN_TYPE align_type;
    int fontOID;
    bool bUpdate;

    virtual void initialize(AeXMLNode *_property, AeUIComponentBase *_owner);
    virtual void clear();
    virtual void updatePreRender();
    virtual void updatePostRedner() {}
};

class AeUIComponentBase {
    REQUIRED_MANAGER_KEY(Base, UI);

   public:
    ~AeUIComponentBase() {}

    AeBaseData data;
    AeUIItemBase *item;
    void AddEvent(AeUIEventType type, UIEventRegisterFunction function);
    void RemoveEvent(AeUIEventType type);

    AeUIComponentBase *owner = nullptr;

    virtual void initialize(AeXMLNode *_property, AeUIComponentBase *_owner);
    virtual void clear() {}

    virtual void updatePreRender() {}
    virtual void updatePostRedner() {}
};

class AeUIComponentFont : public AeUIComponentBase {
   public:
    AeUIComponentFont(AeUIManagerKey &_key) : AeUIComponentBase(_key) {}
    ~AeUIComponentFont() {}

    HFONT win32Font;

    int fontSize;
    std::string fontStyle;

    virtual void initialize(AeXMLNode *_property, AeUIComponentBase *_owner);
};

class AeUIComponentRedner : public AeUIComponentBase {
   public:
    AeUIComponentRedner(AeUIManagerKey &_key) : AeUIComponentBase(_key) {}
    ~AeUIComponentRedner() {}

    uint32_t sceneEID;
    uint32_t cameraOID;
    //VkSurfaceKHR surface = VK_NULL_HANDLE;

    virtual void initialize(AeXMLNode *_property, AeUIComponentBase *_owner);
};

class AeUIComponentListBox : public AeUIComponentBase {
   public:
    AeUIComponentListBox(AeUIManagerKey &_key) : AeUIComponentBase(_key) {}
    ~AeUIComponentListBox() {}

    bool bSelected, bWordWrap;
    LONG msgMaxWidth;

    virtual void initialize(AeXMLNode *_property, AeUIComponentBase *_owner);

    void msg(std::string &s);
};

class AeUIIComponentXMLEditor : public AeUIComponentBase {
   public:
    AeUIIComponentXMLEditor(AeUIManagerKey &_key) : AeUIComponentBase(_key) {}
    ~AeUIIComponentXMLEditor() {}

    std::string path;
    AeXMLNode *xml;

    HWND tabControlCategory, listViewDetail, currentEditListView;
    std::vector<HWND> treeViewLists;

    virtual void initialize(AeXMLNode *_property, AeUIComponentBase *_owner);
};

class AeUIMgr : public AeLogListener {
    MANAGER_KEY_INSTANCE(UI);
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

const char CONFIG_PATH[] = "data\\config2.xml";
#define CONFIG COM_MGR.getXML(CONFIG_PATH)

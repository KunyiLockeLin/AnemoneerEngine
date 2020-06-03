#include "ui.h"

/*
class AeUIFont : public QeComponent {
   public:
    AeUIFont(AeObjectManagerKey &_key) : QeComponent(_key) {}
    ~AeUIFont() {}

    HFONT win32Font;

    int fontSize;
    std::string fontStyle;

    virtual void initialize(QeAssetXML *_property, QeObject *_owner);
};

class AeUIBase {
   public:
    AeUIBase(AeObjectManagerKey &_key, QeComponent *_owner) : owner(_owner) {}
    ~AeUIBase() {}

    QeComponent *owner = nullptr;
    HINSTANCE win32Instance;
    HWND win32Handle;
    HDC win32DeviceContext;

    QeVector2i position, size;
    AeUIAlignType align;
    int fontOID;
    bool bUpdate;

    virtual void initialize(QeAssetXML *_property, QeObject *_parent);
    virtual void clear();
    virtual void update1();
    virtual void update2() {}
};

class AeUIComponent : public AeUIBase, public QeComponent {
   public:
    AeUIComponent(AeObjectManagerKey &_key) : AeUIBase(_key, this), QeComponent(_key) {}
    ~AeUIComponent() {}

    virtual void initialize(QeAssetXML *_property, QeObject *_parent);
    virtual void clear();
    virtual void update1();
    virtual void update2();
};

class AeUIRedner : public AeUIComponent {
   public:
    AeUIRedner(AeObjectManagerKey &_key) : AeUIComponent(_key) {}
    ~AeUIRedner() {}

    uint32_t sceneEID;
    uint32_t cameraOID;
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    virtual void initialize(QeAssetXML *_property, QeObject *_owner);
};

class AeUIListBox : public AeUIComponent {
   public:
    AeUIListBox(AeObjectManagerKey &_key) : AeUIComponent(_key) {}
    ~AeUIListBox() {}

    bool bSelected, bWordWrap;
    LONG msgMaxWidth;

    virtual void initialize(QeAssetXML *_property, QeObject *_parent);

    void msg(std::string &s);
};

class AeUIXMLEditor : public AeUIComponent {
   public:
    AeUIXMLEditor(AeObjectManagerKey &_key) : AeUIComponent(_key) {}
    ~AeUIXMLEditor() {}

    std::string path;
    QeAssetXML *xml;

    HWND tabControlCategory, listViewDetail, currentEditListView;
    std::vector<HWND> treeViewLists;

    virtual void initialize(QeAssetXML *_property, QeObject *_parent);
};

class AeUIWindow : public AeUIBase, public QeObject {
   public:
    AeUIWindow(AeObjectManagerKey &_key) : AeUIBase(_key, this), QeObject(_key) {}
    ~AeUIWindow() {}

    virtual void initialize(QeAssetXML *_property, QeObject *_parent);
    virtual void clear();
    virtual void update1();
    virtual void update2();

    void resize(HWND targetHandle);
};

class AeUI : public QeObject {
   public:
    AeUI(AeObjectManagerKey &_key) : QeObject(_key) {}
    ~AeUI() {}

    AeInputData inputData;
    std::vector<QeInputControl *> inputControls;
    std::vector<AeUIListBox *> listboxs;
    QeVector2i screenSize;
    bool bResize = false;

    virtual void initialize(QeAssetXML *_property, QeObject *_parent);
    virtual void clear();
    virtual void update1();

    void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void resize(HWND targetHandle);

    void Log(std::string &s);
    std::wstring chartowchar(std::string s);
    std::string wchartochar(std::wstring s);
};
*/

/*
void AeUIFont::initialize(QeAssetXML *_property, QeObject *_parent) {
    QeComponent::initialize(_property, _parent);

    fontSize = initProperty->getXMLValuei("size");
    fontStyle = initProperty->getXMLValue("style");
    std::wstring ws = UI->chartowchar(fontStyle);

    win32Font = CreateFont(fontSize, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                           CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, ws.c_str());
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (UI) UI->handleMessages(hWnd, uMsg, wParam, lParam);
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void AeUIBase::initialize(QeAssetXML *_property, QeObject *_parent) {
    win32Instance = GetModuleHandle(nullptr);
    position = _property->getXMLValueiXY("position");
    size = _property->getXMLValueiXY("size");
    align = (AeUIAlignType)_property->getXMLValuei("align");
    fontOID = _property->getXMLValuei("font_oid");

    bUpdate = false;
}

void AeUIBase::clear() { DestroyWindow(win32Handle); }

void AeUIBase::update1() {
    if (bUpdate) return;
    bUpdate = true;

    QeVector2i parent_size = {};
    AeUIWindow *p = (AeUIWindow *)owner->parent;
    while (p) {
        if (!p->parent) {
            if (parent_size.x == 0) {
                parent_size.x = UI->screenSize.x;
            }
            if (parent_size.y == 0) {
                parent_size.y = UI->screenSize.y;
            }
            break;
        } else {
            if (p->size.x != 0 && parent_size.x == 0) {
                parent_size.x = p->size.x;
            }
            if (p->size.y != 0 && parent_size.y == 0) {
                parent_size.y = p->size.y;
            }
            if (parent_size.x != 0 && parent_size.y != 0) {
                break;
            }
        }
        p = (AeUIWindow *)p->parent;
    }
    RECT rect;
    rect.left = position.x;
    rect.top = position.y;
    rect.right = size.x + position.x;
    rect.bottom = size.y + position.y;
    if (rect.right == 0) rect.right = parent_size.x;
    if (rect.bottom == 0) rect.bottom = parent_size.y;
    if (owner->parent == owner->root) {
        DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        AdjustWindowRectEx(&rect, dwStyle, false, dwExStyle);
    }
    switch (align) {
        case eUIAlign_top_right:
        case eUIAlign_middle_right:
        case eUIAlign_bottom_right:
            rect.left += (parent_size.x - size.x);
            rect.right += (parent_size.x - size.x);
            break;
        case eUIAlign_top_middle:
        case eUIAlign_middle:
        case eUIAlign_bottom_middle:
            rect.left += (parent_size.x - size.x) / 2;
            rect.right += (parent_size.x - size.x) / 2;
            break;
        default:
            break;
    }
    switch (align) {
        case eUIAlign_bottom_left:
        case eUIAlign_bottom_middle:
        case eUIAlign_bottom_right:
            rect.top += (parent_size.y - size.y);
            rect.bottom += (parent_size.y - size.y);
            break;
        case eUIAlign_middle_left:
        case eUIAlign_middle:
        case eUIAlign_middle_right:
            rect.top += (parent_size.y - size.y) / 2;
            rect.bottom += (parent_size.y - size.y) / 2;
            break;
        default:
            break;
    }

    SetWindowPos(win32Handle, 0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);

    if (fontOID) {
        AeUIFont *font = (AeUIFont *)OBJMGR->findComponent(eUIComponent_font, fontOID);
        SendMessage(win32Handle, WM_SETFONT, WPARAM(font->win32Font), TRUE);
        SelectObject(win32DeviceContext, font->win32Font);
    }
}

void AeUIListBox::initialize(QeAssetXML *_property, QeObject *_parent) {
    AeUIComponent::initialize(_property, _parent);

    bSelected = initProperty->getXMLValueb("selected");
    bWordWrap = initProperty->getXMLValueb("word_wrap");
    std::wstring title = UI->chartowchar(name);

    AeUIWindow *p = (AeUIWindow *)parent;
    win32Handle = CreateWindow(WC_LISTBOX, title.c_str(), WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL, 0, 0, 0, 0,
                               p->win32Handle, NULL, win32Instance, NULL);
    SetWindowText(win32Handle, title.c_str());
    win32DeviceContext = GetDC(win32Handle);

    AeUI *ui_root = (AeUI *)root;
    ui_root->listboxs.emplace_back(this);
    msgMaxWidth = 0;
}

void AeUIComponent::initialize(QeAssetXML *_property, QeObject *_parent) {
    AeUIBase::initialize(_property, _parent);
    QeComponent::initialize(_property, _parent);
}

void AeUIRedner::initialize(QeAssetXML *_property, QeObject *_parent) {
    AeUIComponent::initialize(_property, _parent);

    sceneEID = initProperty->getXMLValuei("sceneEID");
    cameraOID = initProperty->getXMLValuei("cameraOID");
}

void AeUIXMLEditor::initialize(QeAssetXML *_property, QeObject *_parent) {
    AeUIComponent::initialize(_property, _parent);

    path = initProperty->getXMLValue("path");
    xml = AST->getXML(path.c_str());
    std::wstring title = UI->chartowchar(name);

    AeUIWindow *p = (AeUIWindow *)parent;
    win32Handle = CreateWindow(WC_LISTBOX, title.c_str(), WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL, 0, 0, 0, 0,
                               p->win32Handle, NULL, win32Instance, NULL);
    SetWindowText(win32Handle, title.c_str());
    win32DeviceContext = GetDC(win32Handle);

    tabControlCategory = CreateWindow(WC_TABCONTROL, L"", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, width + 10, height + 10,
                                      win32Handle, NULL, windowInstance, NULL);
    TCITEM tie;
    tie.mask = TCIF_TEXT;
    treeViewLists.clear();
    int i = 0;
    for (const auto &it : CONFIG->nexts) {
        std::wstring ws = chartowchar(it->key);
        tie.pszText = const_cast<LPWSTR>(ws.c_str());
        TabCtrl_InsertItem(tabControlCategory, i, &tie);
        treeViewLists.emplace_back(CreateWindow(WC_TREEVIEW, L"", WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL, 0, 25,
                                                width / 2 - 200, height - 30, editPanel, NULL, windowInstance, NULL));
        SendMessage(treeViewLists[i], WM_SETFONT, WPARAM(hFont), TRUE);
        ++i;
    }
    currentTabIndex = 0;

    listViewDetail = CreateWindow(
        WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | LVS_REPORT | LVS_NOCOLUMNHEADER | LVS_EDITLABELS,
        width / 2 - 200, 25, width / 2 + 100, height - 30, editPanel, NULL, windowInstance, NULL);
    SendMessage(listViewDetail, WM_SETFONT, WPARAM(hFont), TRUE);

    LVCOLUMN lvc;
    lvc.mask = LVCF_WIDTH | LVCF_SUBITEM;
    lvc.cx = 200;
    lvc.iSubItem = 0;
    ListView_InsertColumn(listViewDetail, 0, &lvc);

    lvc.cx = 395;
    lvc.iSubItem = 1;
    ListView_InsertColumn(listViewDetail, 1, &lvc);

    setAllTreeView();
}
}

void AeUIComponent::clear() {
    AeUIBase::clear();
    QeComponent::clear();
}

void AeUIComponent::update1() {
    AeUIBase::update1();
    QeComponent::update1();
}

void AeUIComponent::update2() {
    AeUIBase::update2();
    QeComponent::update2();
}

void AeUIListBox::msg(std::string &s) {
    std::wstring ws = UI->chartowchar(s);
    SendMessage(win32Handle, LB_INSERTSTRING, 0, (LPARAM)ws.c_str());
    SIZE textWidth;
    GetTextExtentPoint32(win32DeviceContext, ws.c_str(), int(ws.length()), &textWidth);
    if (textWidth.cx > msgMaxWidth) {
        msgMaxWidth = textWidth.cx;
        SendMessage(win32Handle, LB_SETHORIZONTALEXTENT, msgMaxWidth + 30, 0);
    }
}

void AeUIWindow::initialize(QeAssetXML *_property, QeObject *_parent) {
    AeUIBase::initialize(_property, _parent);
    QeComponent::initialize(_property, _parent);

    std::wstring title = UI->chartowchar(name);
    WNDCLASSEX wndClass;
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = win32Instance;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = title.c_str();
    wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

    RegisterClassEx(&wndClass);

    HWND parentHandle = NULL;
    if (parent && parent->componentType == eWindow) {
        parentHandle = ((AeUIWindow *)parent)->win32Handle;
    }
    win32Handle = CreateWindowEx(0, title.c_str(), 0, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 0, 0,
                                 parentHandle, NULL, win32Instance, NULL);
    SetWindowText(win32Handle, title.c_str());
    ShowWindow(win32Handle, SW_SHOW);

    QeObject::initialize(_property, _parent);
}

void AeUIWindow::clear() {
    AeUIBase::clear();
    QeObject::clear();
}

void AeUIWindow::update1() {
    AeUIBase::update1();
    QeObject::update1();
}

void AeUIWindow::update2() {
    AeUIBase::update2();
    QeObject::update2();
}

void AeUIWindow::resize(HWND targetHandle) {
    if (targetHandle == NULL || targetHandle == win32Handle) {
        if (targetHandle == NULL) bUpdate = false;
        RECT rect;
        GetClientRect(win32Handle, &rect);
        if (size.x != 0) size.x = rect.right - rect.left;
        if (size.y != 0) size.y = rect.bottom - rect.top;
        for (const auto component : components) {
            ((AeUIComponent *)component)->bUpdate = false;
        }
        for (const auto child : children) {
            ((AeUIWindow *)child)->resize(NULL);
        }
    }
}

void AeUI::initialize(QeAssetXML *_property, QeObject *_parent) {
    QeComponent::initialize(_property, _parent);
    for (const auto node : initProperty->nexts) {
        children.push_back(OBJMGR->spwanComponent(node, this));
    }
    screenSize.x = GetSystemMetrics(SM_CXSCREEN);
    screenSize.y = GetSystemMetrics(SM_CYSCREEN);
}

void AeUI::clear() {
    QeObject::clear();
    inputControls.clear();
    listboxs.clear();
}

void AeUI::update1() {
    QeObject::update1();

    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void AeUI::resize(HWND targetHandle) {
    screenSize.x = GetSystemMetrics(SM_CXSCREEN);
    screenSize.y = GetSystemMetrics(SM_CYSCREEN);

    for (const auto node : children) {
        ((AeUIWindow *)node)->resize(targetHandle);
    }
}

void AeUI::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    inputData.inputType = uMsg;
    inputData.inputKey = int(wParam);
    inputData.mousePos.x = LOWORD(lParam);
    inputData.mousePos.y = HIWORD(lParam);

    switch (uMsg) {
        case WM_CLOSE:
            ENGINE->bClosed = true;
            break;
        case WM_SIZE:
            if (bResize) {
                bResize = false;
                GRAP->bRecreateRender = true;
                resize(hWnd);
            } else {
                bResize = true;
            }
            break;
        case WM_SYSCOMMAND: {
                int sc_wparam = GET_SC_WPARAM(wParam);
                if (sc_wparam == SC_MAXIMIZE || sc_wparam == SC_RESTORE) {
                    bResize = true;
                }
            }
            break;
        //case WM_WINDOWPOSCHANGED:
        //    if (bResize) {
        //        bResize = false;
        //        LOG("WM_WINDOWPOSCHANGED ttttttttttttttttt");
        //        //resize(hWnd);
        //    }
        //    break;
        
case WM_EXITSIZEMOVE:
    if (bResize) {
        bResize = false;
        GRAP->bRecreateRender = true;
        resize(hWnd);
    }
    break;
default:

    switch (wParam) {
        case VK_ESCAPE:
            if (uMsg != WM_IME_COMPOSITION) ENGINE->bClosed = true;
            break;
        default:
            for (const auto input : inputControls) {
                input->updateInput();
            }
            break;
    }
    break;
    }
    }

    void AeUI::Log(std::string &s) {
        for (const auto list : listboxs) {
            list->msg(s);
        }
    }

    std::wstring AeUI::chartowchar(std::string s) {
        wchar_t rtn[4096];
        size_t outSize;
        mbstowcs_s(&outSize, rtn, 4096, s.c_str(), s.length());
        return rtn;
    }

    std::string AeUI::wchartochar(std::wstring s) {
        char rtn[4096];
        size_t outSize;
        wcstombs_s(&outSize, rtn, 4096, s.c_str(), s.length());
        return rtn;
    }
*/

void AeUIComponentBase::AddEvent(AeUIEventType type, UIEventRegisterFunction function) {
    UI_MGR.AddEvent(type, data.oid, function);
}

void AeUIComponentBase::RemoveEvent(AeUIEventType type) { UI_MGR.RemoveEvent(type, data.oid); }

SINGLETON_INSTANCE(AeUIMgr)

LRESULT EditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CHAR:
            switch (wParam) {
                case KEY_FSLASH:
                    UI_MGR.closeCommand();
                    break;
                case VK_RETURN:
                    UI_MGR.sendCommand();
                    UI_MGR.closeCommand();
                    return true;
                    break;
            }
        default:
            return CallWindowProc(UI_MGR.DefEditProc, hwnd, uMsg, wParam, lParam);
    }
    return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    UI_MGR.handleMessages(hWnd, uMsg, wParam, lParam);
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

AeUIMgr::AeUIMgr(){}
AeUIMgr::~AeUIMgr() { LOGOBJ.removeListener(*this); }

void AeUIMgr::closeCommand() {
    ShowWindow(commandBox, SW_HIDE);
    SetFocus(mainWindow);
}

void AeUIMgr::sendCommand() {
    wchar_t lpString[256];
    GetWindowText(commandBox, lpString, 256);
    for (auto *listener : command_listeners) {
        listener->updateCommand(wchartochar(lpString));
    }
}

void AeUIMgr::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (hWnd == editPanel) {
        switch (uMsg) {
            case WM_NOTIFY:
                switch (((LPNMHDR)lParam)->code) {
                    case TCN_SELCHANGE:
                        updateTab();
                        break;
                    case TVN_SELCHANGED:
                        updateListView();
                        break;
                    case LVN_BEGINLABELEDIT: {
                        currentEditListView = ListView_GetEditControl(listViewDetail);
                        TCHAR text[512] = L"";

                        GetWindowText(currentEditListView, text, sizeof(text));
                        currentEditListViewKey = wchartochar(text);
                    } break;
                    case LVN_ENDLABELEDIT:
                        updateListViewItem();
                        break;
                }
                break;

            case WM_COMMAND:
                switch (wParam) {
                    case eUIType_btnPause:
                        // ENGINE->bPause = !ENGINE->bPause;
                        break;
                    case eUIType_btnUpdateAll:
                        // ENGINE->initialize();
                        break;
                    case eUIType_btnLoadAll:
                        COM_MGR.removeXML(CONFIG_PATH);
                        // ENGINE->initialize();
                        // updateTab();
                        setAllTreeView();
                        break;
                    case eUIType_btnSaveAll: {
                        adjustComponetData(CONFIG);
                        CONFIG->outputXML(CONFIG_PATH);
                        // ENGINE->initialize();
                    } break;
                    case eUIType_btnLoadScene:
                        if (currentTreeViewNode) {
                            int type = currentTreeViewNode->getXMLValue<int>("type");
                            if (type == eGAMEOBJECT_Scene) {
                                ID eid = currentTreeViewNode->getXMLValue<ID>("eid");
                                // OBJMGR->loadScene(eid);
                            }
                        }
                        break;
                    case eUIType_btnSaveEID:
                        if (currentTreeViewNode) {
                            int _type = currentTreeViewNode->getXMLValue<int>("type");
                            ID _eid = currentTreeViewNode->getXMLValue<ID>("eid");
                            if (_type != 0 && _eid != 0) {
                                /*AeXMLNode *node = G_AST.getXMLEditNode((AE_GAMEOBJECT_TYPE)_type, _eid);
                                if (node)
                                    currentTreeViewNode->copyXMLNode(node);
                                else {
                                    AeXMLNode *node = G_AST.getXMLEditNode((AE_GAMEOBJECT_TYPE)_type, 0);
                                    AeXMLNode *newNode = currentTreeViewNode->copyXMLNode();
                                    node->data->parent->addXMLNode(newNode);
                                }*/
                            }
                        }
                        break;
                    case eUIType_btnLoadEID:
                        if (currentTreeViewNode) {
                            int _type = currentTreeViewNode->getXMLValue<int>("type");
                            ID _eid = currentTreeViewNode->getXMLValue<ID>("eid");
                            if (_type != 0) {
                                /*AeXMLNode *node = G_AST.getXMLEditNode((AE_GAMEOBJECT_TYPE)_type, _eid);
                                if (node) {
                                    HTREEITEM hSelectedItem = TreeView_GetSelection(treeViewLists[currentTabIndex]);

                                    while (1) {
                                        HTREEITEM hChild = TreeView_GetChild(treeViewLists[currentTabIndex], hSelectedItem);
                                        if (!hChild) break;
                                        TreeView_DeleteItem(treeViewLists[currentTabIndex], hChild);
                                    }

                                    node->copyXMLNode(currentTreeViewNode);
                                    // updateTab();
                                    setTreeViewText(hSelectedItem, currentTreeViewNode);
                                    for (const auto n : currentTreeViewNode->data->nexts) {
                                        addToTreeView(n, hSelectedItem);
                                    }
                                    updateListView();
                                }*/
                            }
                        }
                        break;
                    case eUIType_btnCameraFocus:
                        if (currentTreeViewNode) {
                            int type = currentTreeViewNode->getXMLValue<int>("type");
                            if (type == eGAMEOBJECT_Component_Transform) {
                                ID oid = currentTreeViewNode->getXMLValue<ID>("oid");
                                // GRAP->getTargetCamera()->setLookAtTransformOID(oid);
                            }
                        }
                        break;
                    case eUIType_btnCameraControl:
                        if (currentTreeViewNode) {
                            int type = currentTreeViewNode->getXMLValue<int>("type");
                            if (type == eGAMEOBJECT_Component_Camera) {
                                ID oid = currentTreeViewNode->getXMLValue<ID>("oid");
                                // GRAP->setTargetCamera(oid);
                            }
                        }
                        break;
                    case eUIType_btnNewItem:
                        if (currentTreeViewNode) {
                            int _type = currentTreeViewNode->getXMLValue<int>("type");
                            if (_type != 0) {
                                /*AeXMLNode *node = G_AST.getXMLEditNode((AE_GAMEOBJECT_TYPE)_type, 0);
                                AeXMLNode *newNode = node->copyXMLNode();
                                newNode->data->key = "new";
                                if (currentTreeViewNode != node->data->parent) {
                                    currentTreeViewNode->data->parent->addXMLNode(newNode);

                                    HTREEITEM hSelectedItem = TreeView_GetSelection(treeViewLists[currentTabIndex]);
                                    HTREEITEM hParent = TreeView_GetParent(treeViewLists[currentTabIndex], hSelectedItem);

                                    setTreeViewText(hParent, currentTreeViewNode->data->parent);
                                    addToTreeView(newNode, hParent);
                                } else {
                                    currentTreeViewNode->addXMLNode(newNode);
                                    HTREEITEM hSelectedItem = TreeView_GetSelection(treeViewLists[currentTabIndex]);

                                    setTreeViewText(hSelectedItem, currentTreeViewNode);
                                    addToTreeView(newNode, hSelectedItem);
                                }*/
                                // updateTab();
                            } else {
                                AeXMLNode *node = nullptr;
                                if (currentTreeViewNode->data->key.compare("children") == 0) {
                                    // node = G_AST.getXMLEditNode(eGAMEOBJECT_Object, 0);
                                } else if (currentTreeViewNode->data->key.compare("components") == 0) {
                                    // node = G_AST.getXMLEditNode(eGAMEOBJECT_Component_Transform, 0);
                                }
                                if (node) {
                                    AeXMLNode *newNode = node->copyXMLNode();
                                    newNode->data->key = "new";
                                    currentTreeViewNode->addXMLNode(newNode);
                                    // updateTab();
                                    HTREEITEM hSelectedItem = TreeView_GetSelection(treeViewLists[currentTabIndex]);
                                    setTreeViewText(hSelectedItem, currentTreeViewNode);

                                    addToTreeView(newNode, hSelectedItem);
                                }
                            }
                        } else {
                            AeXMLNode *node1 = CONFIG->data->nexts[currentTabIndex];
                            int _type = node1->getXMLValue<int>("type");
                            if (_type != 0) {
                                /*AeXMLNode *node = G_AST.getXMLEditNode((AE_GAMEOBJECT_TYPE)_type, 0);
                                AeXMLNode *newNode = node->copyXMLNode();
                                newNode->data->key = "new";
                                node1->addXMLNode(newNode);
                                // updateTab();
                                addToTreeView(newNode, TVI_FIRST);*/
                            }
                        }

                        break;
                    case eUIType_btnDeleteItem:
                        if (currentTreeViewNode) {
                            CONFIG->removeXMLNode(currentTreeViewNode);
                            currentTreeViewNode = nullptr;
                            // updateTab();
                            HTREEITEM hSelectedItem = TreeView_GetSelection(treeViewLists[currentTabIndex]);
                            TreeView_DeleteItem(treeViewLists[currentTabIndex], hSelectedItem);
                            ListView_DeleteAllItems(listViewDetail);
                        }
                        break;
                }
                break;
        }
    }

    if (hWnd == mainWindow) {
        inputData.inputType = uMsg;
        inputData.inputKey = int(wParam);
        inputData.mousePos.x = LOWORD(lParam);
        inputData.mousePos.y = HIWORD(lParam);

        switch (inputData.inputType) {
            case WM_CLOSE:
                // ENGINE->bClosed = true;
                break;
            case WM_EXITSIZEMOVE:
                // GRAP->bRecreateRender = true;
                break;

            default:

                switch (inputData.inputKey) {
                    case VK_ESCAPE:
                        // if (uMsg != WM_IME_COMPOSITION) ENGINE->bClosed = true;
                        break;
                    case KEY_FSLASH:
                        SetWindowText(commandBox, L"");
                        ShowWindow(commandBox, SW_SHOW);
                        SetFocus(commandBox);
                    default:
                        for (auto *listener : input_listeners) {
                            listener->updateInput(inputData);
                        }
                        break;
                }
                break;
        }
    }
}

void AeUIMgr::setTreeViewText(HTREEITEM hItem, AeXMLNode *node) {
    std::string s = node->data->key;
    s += " ";
    s += std::to_string(node->data->nexts.size());
    std::wstring ws = chartowchar(s);

    TVITEM item;
    item.hItem = hItem;
    item.mask = TVIF_TEXT;
    item.cchTextMax = 512;
    item.pszText = const_cast<LPWSTR>(ws.c_str());
    TreeView_SetItem(treeViewLists[currentTabIndex], &item);
}

void AeUIMgr::adjustComponetData(AeXMLNode *node) {
    int _type = node->getXMLValue<int>("type");
    if (_type) {
        /* AeXMLNode *source = G_AST.getXMLEditNode((AE_GAMEOBJECT_TYPE)_type, 0);
         if (source->data->parent != node) {
             auto elements = source->data->elements;
             for (auto &e : elements) {
                 for (auto &e1 : node->data->elements) {
                     if (e.key.compare(e1.key) == 0) {
                         e.value = e1.value;
                     }
                 }
             }
             node->data->elements = elements;
         }*/
    }
    for (const auto &n : node->data->nexts) {
        adjustComponetData(n);
    }
}

void AeUIMgr::getWindowSize(HWND &window, int &width, int &height) {
    RECT rect;
    GetClientRect(window, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
}

void AeUIMgr::resizeAll() {
    resize(logPanel);
    resize(editPanel);
    resize(mainWindow);
}

void AeUIMgr::resize(HWND &window) {
    RECT windowRect;
    windowRect.left = 0;
    windowRect.top = 0;
    int offsetX = 0;
    int offsetY = 0;
    AeXMLNode *node = CONFIG->getXMLNode("setting.environment");
    std::string type = "";
    if (window == mainWindow) {
        windowRect.right = node->getXMLValue<int>("mainWidth");
        windowRect.bottom = node->getXMLValue<int>("mainHeight");
        offsetX = node->getXMLValue<int>("mainOffsetX");
        offsetY = node->getXMLValue<int>("mainOffsetY");
    } else if (window == editPanel) {
        windowRect.right = node->getXMLValue<int>("editWidth");
        windowRect.bottom = node->getXMLValue<int>("editHeight");
        offsetX = node->getXMLValue<int>("editOffsetX");
        offsetY = node->getXMLValue<int>("editOffsetY");
    } else if (window == logPanel) {
        windowRect.right = node->getXMLValue<int>("logWidth");
        windowRect.bottom = node->getXMLValue<int>("logHeight");
        offsetX = node->getXMLValue<int>("logOffsetX");
        offsetY = node->getXMLValue<int>("logOffsetY");
    }

    DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    AdjustWindowRectEx(&windowRect, dwStyle, false, dwExStyle);

    int x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right + windowRect.left) / 2 + offsetX;
    int y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom + windowRect.top) / 2 + offsetY;
    windowRect.right += (x - windowRect.left);
    windowRect.bottom += (y - windowRect.top);
    windowRect.left = x;
    windowRect.top = y;

    SetWindowPos(window, 0, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
                 SWP_NOZORDER);
    // GRAP->bRecreateRender = true;
}

void AeUIMgr::openMainWindow() {
    LOG("create MainWindow");
    WNDCLASSEX wndClass;
    std::wstring title = L"MainWindow";

    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = windowInstance;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = title.c_str();
    wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

    RegisterClassEx(&wndClass);

    mainWindow = CreateWindowEx(0, title.c_str(), 0, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 0, 0, NULL,
                                NULL, windowInstance, NULL);

    resize(mainWindow);

    ShowWindow(mainWindow, SW_SHOW);

    int width;
    int height;
    getWindowSize(mainWindow, width, height);
    commandBox = CreateWindow(WC_EDITW, L"", WS_CHILD, 0, 0, width, 20, mainWindow, (HMENU)1, NULL, NULL);

    ShowWindow(commandBox, SW_HIDE);
    DefEditProc = (WNDPROC)SetWindowLongPtr(commandBox, GWLP_WNDPROC, (LONG_PTR)(EditProc));
}

void AeUIMgr::openEditPanel() {
    LOG("create Edit Panel");
    std::wstring title = L"Edit Panel";
    WNDCLASSEX wndClass;
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = windowInstance;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = title.c_str();
    wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    RegisterClassEx(&wndClass);

    editPanel = CreateWindowEx(0, title.c_str(), 0, WS_CAPTION, 0, 0, 0, 0, NULL, NULL, windowInstance, NULL);
    resize(editPanel);
    ShowWindow(editPanel, SW_SHOW);
    SetWindowText(editPanel, title.c_str());

    int width;
    int height;
    getWindowSize(editPanel, width, height);
    int fontSize = CONFIG->getXMLValue<int>("setting.environment.editFontSize");
    HFONT hFont = CreateFont(fontSize, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");

    tabControlCategory = CreateWindow(WC_TABCONTROL, L"", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, width + 10, height + 10,
                                      editPanel, NULL, windowInstance, NULL);
    TCITEM tie;
    tie.mask = TCIF_TEXT;
    treeViewLists.clear();
    int i = 0;
    for (const auto &it : CONFIG->data->nexts) {
        std::wstring ws = chartowchar(it->data->key);
        tie.pszText = const_cast<LPWSTR>(ws.c_str());
        TabCtrl_InsertItem(tabControlCategory, i, &tie);
        treeViewLists.emplace_back(CreateWindow(WC_TREEVIEW, L"", WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL, 0, 25,
                                                width / 2 - 200, height - 30, editPanel, NULL, windowInstance, NULL));
        SendMessage(treeViewLists[i], WM_SETFONT, WPARAM(hFont), TRUE);
        ++i;
    }
    currentTabIndex = 0;

    listViewDetail = CreateWindow(WC_LISTVIEW, L"",
                                  WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | LVS_REPORT | LVS_NOCOLUMNHEADER |
                                      LVS_EDITLABELS /*| LVS_EX_FULLROWSELECT*/,
                                  width / 2 - 200, 25, width / 2 + 100, height - 30, editPanel, NULL, windowInstance, NULL);
    SendMessage(listViewDetail, WM_SETFONT, WPARAM(hFont), TRUE);

    LVCOLUMN lvc;
    lvc.mask = LVCF_WIDTH | LVCF_SUBITEM;
    lvc.cx = 200;
    lvc.iSubItem = 0;
    // lvc.pszText = _T("Property");
    ListView_InsertColumn(listViewDetail, 0, &lvc);

    lvc.cx = 395;
    lvc.iSubItem = 1;
    // lvc.pszText = _T("Value");
    ListView_InsertColumn(listViewDetail, 1, &lvc);

    btnPause = CreateWindow(WC_BUTTON, L"Pause", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, width - 100, 25, 100, 50,
                            editPanel, (HMENU)eUIType_btnPause, windowInstance, NULL);
    btnUpdateAll = CreateWindow(WC_BUTTON, L"Update All", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, width - 100,
                                25 + 55 * 1, 100, 50, editPanel, (HMENU)eUIType_btnUpdateAll, windowInstance, NULL);
    btnLoadAll = CreateWindow(WC_BUTTON, L"Load All", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, width - 100,
                              25 + 55 * 2, 100, 50, editPanel, (HMENU)eUIType_btnLoadAll, windowInstance, NULL);
    btnSaveAll = CreateWindow(WC_BUTTON, L"Save All", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, width - 100,
                              25 + 55 * 3, 100, 50, editPanel, (HMENU)eUIType_btnSaveAll, windowInstance, NULL);
    btnLoadScene = CreateWindow(WC_BUTTON, L"Load Scene", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, width - 100,
                                25 + 55 * 4, 100, 50, editPanel, (HMENU)eUIType_btnLoadScene, windowInstance, NULL);
    btnSaveEID = CreateWindow(WC_BUTTON, L"Save eid", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, width - 100,
                              25 + 55 * 5, 100, 50, editPanel, (HMENU)eUIType_btnSaveEID, windowInstance, NULL);
    btnLoadEID = CreateWindow(WC_BUTTON, L"Load eid", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, width - 100,
                              25 + 55 * 6, 100, 50, editPanel, (HMENU)eUIType_btnLoadEID, windowInstance, NULL);
    btnCameraFocus = CreateWindow(WC_BUTTON, L"Camera Focus", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, width - 100,
                                  25 + 55 * 7, 100, 50, editPanel, (HMENU)eUIType_btnCameraFocus, windowInstance, NULL);
    btnCameraControl = CreateWindow(WC_BUTTON, L"Camera Ctrl", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, width - 100,
                                    25 + 55 * 8, 100, 50, editPanel, (HMENU)eUIType_btnCameraControl, windowInstance, NULL);
    btnNewItem = CreateWindow(WC_BUTTON, L"New Item", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, width - 100,
                              25 + 55 * 9, 100, 50, editPanel, (HMENU)eUIType_btnNewItem, windowInstance, NULL);
    btnDeleteItem = CreateWindow(WC_BUTTON, L"Delete Item", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, width - 100,
                                 25 + 55 * 10, 100, 50, editPanel, (HMENU)eUIType_btnDeleteItem, windowInstance, NULL);

    setAllTreeView();
    // updateTab();
}

void AeUIMgr::openLogPanel() {
    std::wstring title = L"Log Panel";

    WNDCLASSEX wndClass;
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = windowInstance;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = title.c_str();
    wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

    RegisterClassEx(&wndClass);

    logPanel = CreateWindow(WC_LISTBOX, title.c_str(), WS_CAPTION, 0, 0, 0, 0, NULL, NULL, windowInstance, NULL);

    resize(logPanel);
    ShowWindow(logPanel, SW_SHOW);
    SetWindowText(logPanel, title.c_str());

    int width;
    int height;
    getWindowSize(logPanel, width, height);
    int fontSize = CONFIG->getXMLValue<int>("setting.environment.logFontSize");
    HFONT hFont = CreateFont(fontSize, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");

    listBoxLog = CreateWindow(WC_LISTBOX, L"", WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL, 0, 0, width, height + 6, logPanel,
                              NULL, windowInstance, NULL);
    SendMessage(listBoxLog, WM_SETFONT, WPARAM(hFont), TRUE);
    logHDC = GetDC(listBoxLog);
    SelectObject(logHDC, hFont);
    logMaxWidth = 0;
    LOGOBJ.addListener(*this);
    LOG("create Log Panel");
}

void AeUIMgr::Log(std::string _log) {
    std::wstring ws = chartowchar(_log);
    SendMessage(listBoxLog, LB_INSERTSTRING, 0, (LPARAM)ws.c_str());
    SIZE textWidth;
    GetTextExtentPoint32(logHDC, ws.c_str(), int(ws.length()), &textWidth);
    if (textWidth.cx > logMaxWidth) {
        logMaxWidth = textWidth.cx;
        SendMessage(listBoxLog, LB_SETHORIZONTALEXTENT, logMaxWidth + 30, 0);
    }
    /*std::vector<std::string> vs = ENCODE->split(_log.c_str(), "\n");

    for (int i = 0; i < vs.size(); ++i) {
        LPSTR s = const_cast<char *>(vs[i].c_str());
        std::wstring ws = chartowchar(vs[i]);
        SendMessage(listBoxLog, LB_INSERTSTRING, 0, (LPARAM)ws.c_str());
        SIZE textWidth;
        GetTextExtentPoint32A(GetDC(listBoxLog), s, int(vs[i].length()), &textWidth);
        if (textWidth.cx > logMaxWidth) logMaxWidth = textWidth.cx;
    }
    SendMessage(listBoxLog, LB_SETHORIZONTALEXTENT, logMaxWidth, 0);*/
}

void AeUIMgr::updateListViewItem() {
    TCHAR text[512] = L"";
    GetWindowText(currentEditListView, text, sizeof(text));

    int iIndex = ListView_GetNextItem(listViewDetail, -1, LVNI_FOCUSED);
    ListView_SetItemText(listViewDetail, iIndex, 1, text);

    if (currentEditListViewKey.compare("name") == 0) {
        currentTreeViewNode->setXMLKey(wchartochar(text).c_str());
        // updateTab();
        HTREEITEM hSelectedItem = TreeView_GetSelection(treeViewLists[currentTabIndex]);
        setTreeViewText(hSelectedItem, currentTreeViewNode);
    } else
        currentTreeViewNode->setXMLValue(currentEditListViewKey.c_str(), wchartochar(text).c_str());
}

void AeUIMgr::updateListView() {
    if (bAddTreeView) return;
    HTREEITEM hSelectedItem = TreeView_GetSelection(treeViewLists[currentTabIndex]);
    if (!hSelectedItem) return;

    TCHAR buffer[512];

    TVITEM item;
    item.hItem = hSelectedItem;
    item.mask = TVIF_TEXT;
    item.cchTextMax = 512;
    item.pszText = buffer;

    if (!TreeView_GetItem(treeViewLists[currentTabIndex], &item)) return;

    currentTreeViewNode = (AeXMLNode *)item.lParam;
    ListView_DeleteAllItems(listViewDetail);

    LVITEM lvi;
    lvi.mask = LVIF_TEXT;
    lvi.iItem = 0;
    lvi.iSubItem = 0;
    lvi.pszText = L"name";
    ListView_InsertItem(listViewDetail, &lvi);
    std::wstring ws;

    uint32_t i = 0;
    for (const auto &node : currentTreeViewNode->data->elements) {
        lvi.iItem = i + 1;
        ws = chartowchar(node.key);
        lvi.pszText = const_cast<LPWSTR>(ws.c_str());
        ListView_InsertItem(listViewDetail, &lvi);
        ++i;
    }

    lvi.iItem = 0;
    lvi.iSubItem = 1;
    ws = chartowchar(currentTreeViewNode->data->key);
    lvi.pszText = const_cast<LPWSTR>(ws.c_str());
    ListView_SetItem(listViewDetail, &lvi);

    i = 0;
    for (const auto &node : currentTreeViewNode->data->elements) {
        lvi.iItem = i + 1;
        ws = chartowchar(node.value);
        lvi.pszText = const_cast<LPWSTR>(ws.c_str());
        ListView_SetItem(listViewDetail, &lvi);
        ++i;
    }
}

void AeUIMgr::setAllTreeView() {
    bAddTreeView = true;
    int i = 0;
    for (const auto &it : CONFIG->data->nexts) {
        currentTabIndex = i;
        TreeView_DeleteAllItems(treeViewLists[currentTabIndex]);
        for (const auto &it1 : it->data->nexts) {
            addToTreeView(it1, TVI_FIRST);
        }
        ++i;
    }
    bAddTreeView = false;

    currentTabIndex = 0;

    ListView_DeleteAllItems(listViewDetail);
    currentTreeViewNode = nullptr;
    updateTab();
}

void AeUIMgr::updateTab() {
    currentTabIndex = TabCtrl_GetCurSel(tabControlCategory);

    // TreeView_DeleteAllItems(treeViewLists[currentTabIndex]);
    ListView_DeleteAllItems(listViewDetail);
    for (const auto &view : treeViewLists) {
        ShowWindow(view, SW_HIDE);
    }
    ShowWindow(treeViewLists[currentTabIndex], SW_SHOW);
    updateListView();
    // currentTreeViewNode = nullptr;
}

void AeUIMgr::addToTreeView(AeXMLNode *node, HTREEITEM parent) {
    TVITEM tvi;
    TVINSERTSTRUCT tvins;

    tvi.mask = TVIF_TEXT | TVIF_PARAM;

    std::string s = node->data->key;
    s += " ";
    s += std::to_string(node->data->nexts.size());
    std::wstring ws = chartowchar(s);

    tvi.pszText = const_cast<LPWSTR>(ws.c_str());
    tvi.cchTextMax = 256;
    tvi.lParam = (LPARAM)node;

    tvins.item = tvi;

    // tvins.hInsertAfter = nullptr;

    if (parent == TVI_FIRST)
        tvins.hParent = TVI_ROOT;
    else
        tvins.hParent = parent;

    HTREEITEM item = TreeView_InsertItem(treeViewLists[currentTabIndex], &tvins);
    for (const auto &it : node->data->nexts) {
        addToTreeView(it, item);
    }
}

void AeUIMgr::initialize() {
    if (bInit) return;
    bInit = true;

    windowInstance = GetModuleHandle(nullptr);

    openLogPanel();
    openEditPanel();
    openMainWindow();

    SetForegroundWindow(mainWindow);
    SetFocus(mainWindow);
}

std::string AeUIMgr::getWindowTitle() {
    std::string windowTitle = "";
    /*std::string device(VK->deviceProperties.deviceName);
    AeXMLNode *node = CONFIG->getXMLNode("setting.application");
    windowTitle = node->getXMLValue<std::string>("applicationName");
    windowTitle.append(" ");
    windowTitle.append(node->getXMLValue<std::string>("applicationVersion"));
    windowTitle.append(" - ");
    windowTitle.append(node->getXMLValue<std::string>("engineName"));
    windowTitle.append(" ");
    windowTitle.append(node->getXMLValue<std::string>("engineVersion"));
    windowTitle.append(" - VulkanAPI ");
    windowTitle.append(node->getXMLValue<std::string>("VulkanAPIVersion"));
    windowTitle.append(" - ");
    windowTitle.append(device);
    windowTitle.append(" - ");
    windowTitle.append(std::to_string(ENGINE->currentFPS));
    windowTitle.append("/");
    windowTitle.append(std::to_string(ENGINE->FPS));
    windowTitle.append(" FPS - ");
    windowTitle.append(SCENE->data.name);
    windowTitle.append(" ");
    windowTitle.append(std::to_string(SCENE->data.eid));*/
    return windowTitle;
}

void AeUIMgr::update1() {
    std::string windowTitle = getWindowTitle();
    std::wstring ws = chartowchar(windowTitle);
    SetWindowText(mainWindow, ws.c_str());

    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    // consoleInput();
}

void AeUIMgr::update2() {}

/*void QeWindow::consoleInput() {
    if (!DEBUG->isConsole()) return;

    if (!_kbhit()) return;
    char cur = _getch();

    switch (cur) {
        case VK_ESCAPE:
            EbClosed = true;
            break;
        case VK_RETURN:
            std::cout << std::endl;
            if (inputData.consoleCommandInput.length() > 0) CMD(inputData.consoleCommandInput);
            inputData.consoleCommandInput.clear();
            break;
        case VK_BACK:
            if (!inputData.consoleCommandInput.empty()) {
                inputData.consoleCommandInput.pop_back();
                std::cout << "\b"
                          << " "
                          << "\b";
            }
            break;
        default:
            inputData.consoleCommandInput += cur;
            std::cout << cur;
            break;
    }*/

// hout = GetStdHandle(STD_OUTPUT_HANDLE);
// initializeConsole(hout);
/*HANDLE input_handle = GetStdHandle(STD_INPUT_HANDLE);
INPUT_RECORD input_record;
DWORD input_size = 1;
DWORD events = 0;
PeekConsoleInput(input_handle, &input_record, input_size, &events);
if (events <= 0) return;

ReadConsoleInput(input_handle, &input_record, input_size, &events);
if (input_record.EventType != KEY_EVENT ||
!input_record.Event.KeyEvent.bKeyDown) return;

switch (input_record.Event.KeyEvent.wVirtualKeyCode) {
case VK_ESCAPE:
        ENGINE->bClosed = true;
        break;
case VK_RETURN:
        std::string str;
        //std::getline(std::cin, str);
        if(str.length()>0)	CMD(str);
        break;
}*/
//}

std::wstring AeUIMgr::chartowchar(std::string s) {
    wchar_t rtn[4096];
    size_t outSize;
    mbstowcs_s(&outSize, rtn, 4096, s.c_str(), s.length());
    return rtn;
}

std::string AeUIMgr::wchartochar(std::wstring s) {
    char rtn[4096];
    size_t outSize;
    wcstombs_s(&outSize, rtn, 4096, s.c_str(), s.length());
    return rtn;
}

#include "header.h"

LRESULT EditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CHAR:
            switch (wParam) {
                case KEY_FSLASH:
                    UI->closeCommand();
                    break;
                case VK_RETURN:
                    UI->sendCommand();
                    UI->closeCommand();
                    return true;
                    break;
            }
        default:
            return CallWindowProc(UI->DefEditProc, hwnd, uMsg, wParam, lParam);
    }
    return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    UI->handleMessages(hWnd, uMsg, wParam, lParam);
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

AeUI::~AeUI() { LOGOBJ->removeListener(*this); }

void AeUI::closeCommand() {
    ShowWindow(commandBox, SW_HIDE);
    SetFocus(mainWindow);
}

void AeUI::sendCommand() {
    wchar_t lpString[256];
    GetWindowText(commandBox, lpString, 256);
    CMD(wchartochar(lpString));
}

void AeUI::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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
                        ENGINE->bPause = !ENGINE->bPause;
                        break;
                    case eUIType_btnUpdateAll:
                        ENGINE->initialize();
                        break;
                    case eUIType_btnLoadAll:
                        AST->removeXML(CONFIG_PATH);
                        ENGINE->initialize();
                        // updateTab();
                        setAllTreeView();
                        break;
                    case eUIType_btnSaveAll: {
                        adjustComponetData(CONFIG);
                        CONFIG->outputXML(CONFIG_PATH);
                        ENGINE->initialize();
                    } break;
                    case eUIType_btnLoadScene:
                        if (currentTreeViewNode) {
                            int type = currentTreeViewNode->getXMLValuei("type");
                            if (type == eScene) {
                                int eid = currentTreeViewNode->getXMLValuei("eid");
                                OBJMGR->loadScene(eid);
                            }
                        }
                        break;
                    case eUIType_btnSaveEID:
                        if (currentTreeViewNode) {
                            int _type = currentTreeViewNode->getXMLValuei("type");
                            int _eid = currentTreeViewNode->getXMLValuei("eid");
                            if (_type != 0 && _eid != 0) {
                                AeXMLNode *node = G_AST->getXMLEditNode((QeComponentType)_type, _eid);
                                if (node)
                                    currentTreeViewNode->copyXMLNode(node);
                                else {
                                    AeXMLNode *node = G_AST->getXMLEditNode((QeComponentType)_type, 0);
                                    AeXMLNode *newNode = currentTreeViewNode->copyXMLNode();
                                    node->data->parent->addXMLNode(newNode);
                                }
                            }
                        }
                        break;
                    case eUIType_btnLoadEID:
                        if (currentTreeViewNode) {
                            int _type = currentTreeViewNode->getXMLValuei("type");
                            int _eid = currentTreeViewNode->getXMLValuei("eid");
                            if (_type != 0) {
                                AeXMLNode *node = G_AST->getXMLEditNode((QeComponentType)_type, _eid);
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
                                }
                            }
                        }
                        break;
                    case eUIType_btnCameraFocus:
                        if (currentTreeViewNode) {
                            int type = currentTreeViewNode->getXMLValuei("type");
                            if (type == eComponent_transform) {
                                int oid = currentTreeViewNode->getXMLValuei("oid");
                                GRAP->getTargetCamera()->setLookAtTransformOID(oid);
                            }
                        }
                        break;
                    case eUIType_btnCameraControl:
                        if (currentTreeViewNode) {
                            int type = currentTreeViewNode->getXMLValuei("type");
                            if (type == eComponent_camera) {
                                int oid = currentTreeViewNode->getXMLValuei("oid");
                                GRAP->setTargetCamera(oid);
                            }
                        }
                        break;
                    case eUIType_btnNewItem:
                        if (currentTreeViewNode) {
                            int _type = currentTreeViewNode->getXMLValuei("type");
                            if (_type != 0) {
                                AeXMLNode *node = G_AST->getXMLEditNode((QeComponentType)_type, 0);
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
                                }
                                // updateTab();
                            } else {
                                AeXMLNode *node = nullptr;
                                if (currentTreeViewNode->data->key.compare("children") == 0) {
                                    node = G_AST->getXMLEditNode(eObject, 0);
                                } else if (currentTreeViewNode->data->key.compare("components") == 0) {
                                    node = G_AST->getXMLEditNode(eComponent_transform, 0);
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
                            int _type = node1->getXMLValuei("type");
                            if (_type != 0) {
                                AeXMLNode *node = G_AST->getXMLEditNode((QeComponentType)_type, 0);
                                AeXMLNode *newNode = node->copyXMLNode();
                                newNode->data->key = "new";
                                node1->addXMLNode(newNode);
                                // updateTab();
                                addToTreeView(newNode, TVI_FIRST);
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
                ENGINE->bClosed = true;
                break;
            case WM_EXITSIZEMOVE:
                GRAP->bRecreateRender = true;
                break;

            default:

                switch (inputData.inputKey) {
                    case VK_ESCAPE:
                        if (uMsg != WM_IME_COMPOSITION) ENGINE->bClosed = true;
                        break;
                    case KEY_FSLASH:
                        SetWindowText(commandBox, L"");
                        ShowWindow(commandBox, SW_SHOW);
                        SetFocus(commandBox);
                    default:
                        std::vector<QeInputControl *>::iterator it = inputControls.begin();
                        while (it != inputControls.end()) {
                            (*it)->updateInput();
                            ++it;
                        }
                        break;
                }
                break;
        }
    }
}

void AeUI::setTreeViewText(HTREEITEM hItem, AeXMLNode *node) {
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

void AeUI::adjustComponetData(AeXMLNode *node) {
    int _type = node->getXMLValuei("type");
    if (_type) {
        AeXMLNode *source = G_AST->getXMLEditNode((QeComponentType)_type, 0);
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
        }
    }
    for (const auto &n : node->data->nexts) {
        adjustComponetData(n);
    }
}

void AeUI::getWindowSize(HWND &window, int &width, int &height) {
    RECT rect;
    GetClientRect(window, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
}

void AeUI::resizeAll() {
    resize(logPanel);
    resize(editPanel);
    resize(mainWindow);
}

void AeUI::resize(HWND &window) {
    RECT windowRect;
    windowRect.left = 0;
    windowRect.top = 0;
    int offsetX = 0;
    int offsetY = 0;
    AeXMLNode *node = CONFIG->getXMLNode("setting.environment");
    std::string type = "";
    if (window == mainWindow) {
        windowRect.right = node->getXMLValuei("mainWidth");
        windowRect.bottom = node->getXMLValuei("mainHeight");
        offsetX = node->getXMLValuei("mainOffsetX");
        offsetY = node->getXMLValuei("mainOffsetY");
    } else if (window == editPanel) {
        windowRect.right = node->getXMLValuei("editWidth");
        windowRect.bottom = node->getXMLValuei("editHeight");
        offsetX = node->getXMLValuei("editOffsetX");
        offsetY = node->getXMLValuei("editOffsetY");
    } else if (window == logPanel) {
        windowRect.right = node->getXMLValuei("logWidth");
        windowRect.bottom = node->getXMLValuei("logHeight");
        offsetX = node->getXMLValuei("logOffsetX");
        offsetY = node->getXMLValuei("logOffsetY");
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
    GRAP->bRecreateRender = true;
}

void AeUI::openMainWindow() {
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
    UI->DefEditProc = (WNDPROC)SetWindowLongPtr(UI->commandBox, GWLP_WNDPROC, (LONG_PTR)(EditProc));
}

void AeUI::openEditPanel() {
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
    int fontSize = CONFIG->getXMLValuei("setting.environment.editFontSize");
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

void AeUI::openLogPanel() {
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
    int fontSize = CONFIG->getXMLValuei("setting.environment.logFontSize");
    HFONT hFont = CreateFont(fontSize, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");

    listBoxLog = CreateWindow(WC_LISTBOX, L"", WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL, 0, 0, width, height + 6, logPanel,
                              NULL, windowInstance, NULL);
    SendMessage(listBoxLog, WM_SETFONT, WPARAM(hFont), TRUE);
    logHDC = GetDC(listBoxLog);
    SelectObject(logHDC, hFont);
    logMaxWidth = 0;
    LOGOBJ->addListener(*this);
}

void AeUI::Log(std::string _log) {
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

void AeUI::updateListViewItem() {
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

void AeUI::updateListView() {
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

void AeUI::setAllTreeView() {
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

void AeUI::updateTab() {
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

void AeUI::addToTreeView(AeXMLNode *node, HTREEITEM parent) {
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

void AeUI::initialize() {
    if (bInit) return;
    bInit = true;

    windowInstance = GetModuleHandle(nullptr);

    openLogPanel();
    openEditPanel();
    openMainWindow();

    SetForegroundWindow(mainWindow);
    SetFocus(mainWindow);
}

std::string AeUI::getWindowTitle() {
    std::string device(VK->deviceProperties.deviceName);
    std::string windowTitle;
    AeXMLNode *node = CONFIG->getXMLNode("setting.application");
    windowTitle = node->getXMLValue("applicationName");
    windowTitle.append(" ");
    windowTitle.append(node->getXMLValue("applicationVersion"));
    windowTitle.append(" - ");
    windowTitle.append(node->getXMLValue("engineName"));
    windowTitle.append(" ");
    windowTitle.append(node->getXMLValue("engineVersion"));
    windowTitle.append(" - VulkanAPI ");
    windowTitle.append(node->getXMLValue("VulkanAPIVersion"));
    windowTitle.append(" - ");
    windowTitle.append(device);
    windowTitle.append(" - ");
    windowTitle.append(std::to_string(ENGINE->currentFPS));
    windowTitle.append("/");
    windowTitle.append(std::to_string(ENGINE->FPS));
    windowTitle.append(" FPS - ");
    windowTitle.append(SCENE->name);
    windowTitle.append(" ");
    windowTitle.append(std::to_string(SCENE->eid));
    return windowTitle;
}

void AeUI::update1() {
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

void AeUI::update2() {}

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
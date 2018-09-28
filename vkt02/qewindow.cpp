#include "qeheader.h"


LRESULT EditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg){
	case WM_CHAR:
		switch (wParam) {
		case KEY_FSLASH:
			WIN->closeCommand();
			break;
		case VK_RETURN:
			WIN->sendCommand();
			WIN->closeCommand();
			return true;
			break;
		}
	default:
		return CallWindowProc(WIN->DefEditProc, hwnd, uMsg, wParam, lParam);
	}
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	WIN->handleMessages(hWnd, uMsg, wParam, lParam);
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void QeWindow::closeCommand() {
	ShowWindow(commandBox, SW_HIDE);
	SetFocus(mainWindow);
}

void QeWindow::sendCommand() {

	wchar_t  lpString[256];
	GetWindowText(commandBox, lpString, 256);
	CMD(wchartochar(lpString));
}

void QeWindow::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (hWnd == editWindow) {
		TCHAR text[512] = L"";
		QeAssetXML * node;

		switch (uMsg) {
		case WM_NOTIFY:
			switch (((LPNMHDR)lParam)->code) {
			case TCN_SELCHANGE:
				updateTab();
				break;
			case TVN_SELCHANGED:
				updateListView();
				break;
			case LVN_BEGINLABELEDIT:
				currentEditListView = ListView_GetEditControl(listViewDetail);
				GetWindowText(currentEditListView, text, sizeof(text));
				currentEditListViewKey = wchartochar(text);
				break;
			case LVN_ENDLABELEDIT:
				updateListViewItem();
				break;
			}
			break;

		case WM_COMMAND:
			switch (wParam) {
			case eUIType_btnPause:
				QE->bPause = !QE->bPause;
				break;
			case eUIType_btnUpdateAll:
				QE->initialize();
				break;
			case eUIType_btnLoadAll:
				AST->removeXML(AST->CONFIG);
				QE->initialize();
				updateTab();
				break;
			case eUIType_btnSaveAll:
				node = AST->getXMLNode(1, AST->CONFIG);
				adjustComponetData(node);
				AST->outputXML(node, AST->CONFIG);
				QE->initialize();
				break;
			case eUIType_btnLoadScene:
				if (currentTreeViewNodeIndex!= INDEX_NONE) {
					int type = 0;
					AST->getXMLiValue(&type, currentTreeViewNodes[currentTreeViewNodeIndex], 1, "type");
					if (type == eScene) {
						int eid = 0;
						AST->getXMLiValue(&eid, currentTreeViewNodes[currentTreeViewNodeIndex], 1, "eid");
						SCENE->loadScene(eid);
					}
				}
				break;
			case eUIType_btnSaveEID:
				if (currentTreeViewNodeIndex != INDEX_NONE) {
					int _type = 0, _eid = 0;
					AST->getXMLiValue(&_type, currentTreeViewNodes[currentTreeViewNodeIndex], 1, "type");
					AST->getXMLiValue(&_eid, currentTreeViewNodes[currentTreeViewNodeIndex], 1, "eid");
					if (_type != 0 && _eid != 0) {
						QeAssetXML * node = AST->getXMLEditNode((QeComponentType)_type, _eid);
						if (node) AST->copyXMLNode(currentTreeViewNodes[currentTreeViewNodeIndex], node);
						else {
							QeAssetXML * node = AST->getXMLEditNode((QeComponentType)_type, 0);
							QeAssetXML * newNode = AST->copyXMLNode(currentTreeViewNodes[currentTreeViewNodeIndex]);
							AST->addXMLNode(node->parent, newNode);
						}
					}
				}
				break;
			case eUIType_btnLoadEID:
				if (currentTreeViewNodeIndex != INDEX_NONE) {
					int _type = 0, _eid = 0;
					AST->getXMLiValue(&_type, currentTreeViewNodes[currentTreeViewNodeIndex], 1, "type");
					AST->getXMLiValue(&_eid, currentTreeViewNodes[currentTreeViewNodeIndex], 1, "eid");
					if (_type != 0) {
						QeAssetXML * node = AST->getXMLEditNode((QeComponentType)_type, _eid);
						if (node) AST->copyXMLNode(node, currentTreeViewNodes[currentTreeViewNodeIndex]);
						updateTab();
					}
				}
				break;
			case eUIType_btnSetCamera:
				if (currentTreeViewNodeIndex != INDEX_NONE) {
					int type=0;
					AST->getXMLiValue(&type, currentTreeViewNodes[currentTreeViewNodeIndex], 1, "type");
					if (type ==eComponent_transform) {
						int oid = 0;
						AST->getXMLiValue(&oid, currentTreeViewNodes[currentTreeViewNodeIndex], 1, "oid");
						GRAP->getTargetCamera()->setLookAtTransformOID(oid);
					}
				}
				break;
			case eUIType_btnNewItem:
				if (currentTreeViewNodeIndex != INDEX_NONE) {
					int _type = 0;
					AST->getXMLiValue(&_type, currentTreeViewNodes[currentTreeViewNodeIndex], 1, "type");
					if (_type != 0) {

						QeAssetXML * node = AST->getXMLEditNode((QeComponentType)_type, 0);
						QeAssetXML * newNode = AST->copyXMLNode(node);
						newNode->key = "new";
						if (currentTreeViewNodes[currentTreeViewNodeIndex] != node->parent ) {
							AST->addXMLNode(currentTreeViewNodes[currentTreeViewNodeIndex]->parent, newNode);
						}
						else {
							AST->addXMLNode(currentTreeViewNodes[currentTreeViewNodeIndex], newNode);
						}
						updateTab();
					}
					else {
						QeAssetXML * node = nullptr;
						if (currentTreeViewNodes[currentTreeViewNodeIndex]->key.compare("children") == 0) {
							node = AST->getXMLEditNode(eObject, 0);
						}
						else if (currentTreeViewNodes[currentTreeViewNodeIndex]->key.compare("components") == 0) {
							node = AST->getXMLEditNode(eComponent_transform, 0);
						}
						if (node) {
							QeAssetXML * newNode = AST->copyXMLNode(node);
							newNode->key = "new";
							AST->addXMLNode(currentTreeViewNodes[currentTreeViewNodeIndex], newNode);
							updateTab();
						}
					}
				}
				else {
					QeAssetXML * node1 = AST->getXMLNode(1, AST->CONFIG);
					node1 = node1->nexts[currentTabIndex];
					int _type = 0;
					AST->getXMLiValue(&_type, node1, 1, "type");
					if (_type != 0) {
						QeAssetXML * node = AST->getXMLEditNode((QeComponentType)_type, 0);
						QeAssetXML * newNode = AST->copyXMLNode(node);
						newNode->key = "new";
						AST->addXMLNode(node1, newNode);
						updateTab();
					}
				}

				break;
			case eUIType_btnDeleteItem:
				if (currentTreeViewNodeIndex != INDEX_NONE) {
					AST->removeXMLNode(AST->getXMLNode(1, AST->CONFIG), currentTreeViewNodes[currentTreeViewNodeIndex]);
					updateTab();
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
			QE->bClosed = true;
			break;
		case WM_EXITSIZEMOVE:
			GRAP->bRecreateRender = true;
			break;

		default:

			switch (inputData.inputKey) {
			case VK_ESCAPE:
				if (uMsg != WM_IME_COMPOSITION)	QE->bClosed = true;
				break;
			case KEY_FSLASH:
				SetWindowText(commandBox, L"");
				ShowWindow(commandBox, SW_SHOW);
				SetFocus(commandBox);
			default:
				std::vector<QeInputControl*>::iterator it = inputControls.begin();
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

void QeWindow::adjustComponetData(QeAssetXML * node) {
	int _type = 0;
	AST->getXMLiValue(&_type, node, 1, "type");
	if (_type != 0) {
		QeAssetXML * source = AST->getXMLEditNode((QeComponentType)_type, 0);

		if (source->parent != node) {

			for (int i = 0; i < node->eKeys.size(); ++i) {
				bool b = false;
				for (int j = 0; j < source->eKeys.size(); ++j) {
					if (node->eKeys[i].compare(source->eKeys[j]) == 0) {
						b = true;
						break;
					}
				}
				if (!b) {
					node->eKeys.erase(node->eKeys.begin() + i);
					node->eValues.erase(node->eValues.begin() + i);
					--i;
				}
			}

			for (int i = 0; i < source->eKeys.size(); ++i) {
				bool b = false;
				for (int j = 0; j < node->eKeys.size(); ++j) {
					if (node->eKeys[j].compare(source->eKeys[i]) == 0) {
						b = true;
						break;
					}
				}
				if (!b) {
					node->eKeys.insert(node->eKeys.begin() + i, source->eKeys[i]);
					node->eValues.insert(node->eValues.begin() + i, source->eValues[i]);
				}
			}
		}
	}

	for (int i = 0;i<node->nexts.size();++i) {
		adjustComponetData(node->nexts[i]);
	}
}


void QeWindow::getWindowSize(HWND & window, int& width, int& height) {

	RECT rect;
	GetClientRect(window, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
}

void QeWindow::resizeAll() {
	resize(mainWindow);
	resize(editWindow);
}

void QeWindow::resize(HWND & window) {
	RECT windowRect;
	windowRect.left = 0;
	windowRect.top = 0;

	if (window == mainWindow) {
		windowRect.right = std::stoi(AST->getXMLValue(4, AST->CONFIG, "setting", "environment", "width"));
		windowRect.bottom = std::stoi(AST->getXMLValue(4, AST->CONFIG, "setting", "environment", "height"));
	}
	else if (window == editWindow) {
		windowRect.right = std::stoi(AST->getXMLValue(4, AST->CONFIG, "setting", "environment", "editWidth"));
		windowRect.bottom = std::stoi(AST->getXMLValue(4, AST->CONFIG, "setting", "environment", "editHeight"));
	}
	DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	AdjustWindowRectEx(&windowRect, dwStyle, false, dwExStyle);

	int x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right + windowRect.left) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom + windowRect.top) / 2 - 10;
	windowRect.right += (x - windowRect.left);
	windowRect.bottom += (y - windowRect.top);
	windowRect.left = x;
	windowRect.top = y;

	SetWindowPos(window, 0, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_NOZORDER);
	GRAP->bRecreateRender = true;
}


void QeWindow::openMainWindow() {
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

	if (!RegisterClassEx(&wndClass)) {
		LOG("Could not register window class!");
		fflush(stdout);
		exit(1);
	}

	/*int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (settings.fullscreen) {
	DEVMODE dmScreenSettings;
	memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
	dmScreenSettings.dmSize = sizeof(dmScreenSettings);
	dmScreenSettings.dmPelsWidth = screenWidth;
	dmScreenSettings.dmPelsHeight = screenHeight;
	dmScreenSettings.dmBitsPerPel = 32;
	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	if ((width != (uint32_t)screenWidth) && (height != (uint32_t)screenHeight)) {
	if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
	if (MessageBox(NULL, "Fullscreen Mode not supported!\n Switch to window mode?", "Error", MB_YESNO | MB_ICONEXCLAMATION) == IDYES) {
	settings.fullscreen = false;
	} else {
	return nullptr;
	}
	}
	}

	}*/

	//DWORD dwStyle;

	/*if (settings.fullscreen){
	dwExStyle = WS_EX_APPWINDOW;
	dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	} else {*/
	//dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	//}
	//currentWidth = std::stoi(AST->getString("winWidth"));
	//currentHeight = std::stoi(AST->getString("winHeight"));
	/*RECT windowRect;
	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = std::stoi(AST->getXMLValue(3, AST->CONFIG, "envir", "width"));//settings.fullscreen ? (long)screenWidth : (long)width;
	windowRect.bottom = std::stoi(AST->getXMLValue(3, AST->CONFIG, "envir", "height"));//settings.fullscreen ? (long)screenHeight : (long)height;
	AdjustWindowRectEx(&windowRect, dwStyle, false, dwExStyle);

	float f = GetSystemMetrics(SM_CXSCREEN);

	windowRect.left = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right+ windowRect.left) / 2;
	windowRect.top = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom+ windowRect.top) / 2;
	windowRect.right += windowRect.left;
	windowRect.bottom += windowRect.top;*/

	mainWindow = CreateWindowEx(0, title.c_str(), 0,
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 0, 0, NULL, NULL, windowInstance, NULL);

	//if (!settings.fullscreen){
	// Center on screen
	//uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right) / 2;
	//uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom) / 2;
	//SetWindowPos(window, 0, x, y, 0, 0, SWP_NOSIZE);
	//}
	resize(mainWindow);

	if (!mainWindow) {
		LOG("Could not create window!");
		fflush(stdout);
		exit(1);
	}

	ShowWindow(mainWindow, SW_SHOW);
	SetForegroundWindow(mainWindow);
	SetFocus(mainWindow);

	int width; int height;
	getWindowSize(mainWindow, width, height);
	commandBox = CreateWindow(WC_EDITW, L"", WS_CHILD, 0, 0, width, 20, mainWindow, (HMENU)1, NULL, NULL);

	ShowWindow(commandBox, SW_HIDE);
	WIN->DefEditProc = (WNDPROC)SetWindowLongPtr(WIN->commandBox, GWLP_WNDPROC, (LONG_PTR)(EditProc));
}

void QeWindow::openEditWindow() {

	WNDCLASSEX wndClass;
	std::wstring title = L"EditPanel";

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

	if (!RegisterClassEx(&wndClass)) {
		LOG("Could not register window class!");
		fflush(stdout);
		exit(1);
	}

	editWindow = CreateWindowEx(0, title.c_str(), 0,
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 0, 0, NULL, NULL, windowInstance, NULL);

	resize(editWindow);

	if (!editWindow) {
		LOG("Could not create window!");
		fflush(stdout);
		exit(1);
	}

	ShowWindow(editWindow, SW_SHOW);
	SetForegroundWindow(editWindow);
	SetWindowText(editWindow, title.c_str());

	int width; int height;
	getWindowSize(editWindow, width, height);

	HFONT hFont = CreateFont(24, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
	
	tabControlCategory = CreateWindow(WC_TABCONTROL, L"", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
		0, 0, width+10, height+10, editWindow, NULL, windowInstance, NULL);

	QeAssetXML * node = AST->getXMLNode(1, AST->CONFIG);

	currentTabIndex = 0;

	TCITEM tie;
	tie.mask = TCIF_TEXT;

	for (int i = 0;i<node->nexts.size(); ++i) {
		std::wstring ws = chartowchar(node->nexts[i]->key);
		tie.pszText = const_cast<LPWSTR>(ws.c_str());
		TabCtrl_InsertItem(tabControlCategory, i, &tie);
	}

	treeViewList = CreateWindow(WC_TREEVIEW, L"", WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL,
		0, 25, width/2-200, height-345, editWindow, NULL, windowInstance, NULL);
	SendMessage(treeViewList, WM_SETFONT, WPARAM(hFont), TRUE);

	listViewDetail = CreateWindow(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL 
		| LVS_REPORT | LVS_NOCOLUMNHEADER| LVS_EDITLABELS /*| LVS_EX_FULLROWSELECT*/,
		width/2- 200, 25, width/2+100, height-345, editWindow, NULL, windowInstance, NULL);
	SendMessage(listViewDetail, WM_SETFONT, WPARAM(hFont), TRUE);

	LVCOLUMN lvc;
	lvc.mask = LVCF_WIDTH | LVCF_SUBITEM;
	lvc.cx = 200;
	lvc.iSubItem = 0;
	//lvc.pszText = _T("Property");
	ListView_InsertColumn(listViewDetail, 0, &lvc);

	lvc.cx = 395;
	lvc.iSubItem = 1;
	//lvc.pszText = _T("Value");
	ListView_InsertColumn(listViewDetail, 1, &lvc);
	
	listBoxLog = CreateWindow(WC_LISTBOX, L"", WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL,
		0, 450, width-100, 320, editWindow, NULL, windowInstance, NULL);

	btnPause = CreateWindow(WC_BUTTON, L"Pause", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		width - 100, 25, 100, 50, editWindow, (HMENU) eUIType_btnPause, windowInstance, NULL);
	btnUpdateAll = CreateWindow(WC_BUTTON, L"Update All", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		width - 100, 25 + 55 * 1, 100, 50, editWindow, (HMENU)eUIType_btnUpdateAll, windowInstance, NULL);
	btnLoadAll = CreateWindow(WC_BUTTON, L"Load All", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		width - 100, 25 + 55 * 2, 100, 50, editWindow, (HMENU)eUIType_btnLoadAll, windowInstance, NULL);
	btnSaveAll = CreateWindow(WC_BUTTON, L"Save All", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		width - 100, 25 + 55 * 3, 100, 50, editWindow, (HMENU)eUIType_btnSaveAll, windowInstance, NULL);
	btnLoadScene = CreateWindow(WC_BUTTON, L"Load Scene", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		width - 100, 25 + 55 * 4, 100, 50, editWindow, (HMENU)eUIType_btnLoadScene, windowInstance, NULL);
	btnSaveEID = CreateWindow(WC_BUTTON, L"Save eid", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		width - 100, 25 + 55 * 5, 100, 50, editWindow, (HMENU)eUIType_btnSaveEID, windowInstance, NULL);
	btnLoadEID = CreateWindow(WC_BUTTON, L"Load eid", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		width - 100, 25 + 55 * 6, 100, 50, editWindow, (HMENU)eUIType_btnLoadEID, windowInstance, NULL);
	btnSetCamera = CreateWindow(WC_BUTTON, L"Set Camera", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		width - 100, 25 + 55 * 7, 100, 50, editWindow, (HMENU)eUIType_btnSetCamera, windowInstance, NULL);
	btnNewItem = CreateWindow(WC_BUTTON, L"New Item", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		width - 100, 25 + 55 * 8, 100, 50, editWindow, (HMENU)eUIType_btnNewItem, windowInstance, NULL);
	btnDeleteItem = CreateWindow(WC_BUTTON, L"Delete Item", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		width - 100, 25 + 55 * 9, 100, 50, editWindow, (HMENU)eUIType_btnDeleteItem, windowInstance, NULL);

	updateTab();
}

void QeWindow::Log( std::string _log ) {

	std::vector<std::string> vs = ENCODE->split(_log.c_str(), "\n");

	for (int i = 0;i<vs.size();++i) {
		std::wstring ws = chartowchar(vs[i]);
		SendMessage(listBoxLog, LB_ADDSTRING, 0, (LPARAM)ws.c_str());
	}
}

void QeWindow::updateListViewItem() {

	TCHAR text[512] = L"";
	GetWindowText(currentEditListView, text, sizeof(text));

	int iIndex = ListView_GetNextItem(listViewDetail, -1, LVNI_FOCUSED);
	LVITEM LvItem;
	LvItem.iSubItem = 1;
	LvItem.pszText = text;

	SendMessage(listViewDetail, LVM_SETITEMTEXT, (WPARAM)iIndex, (LPARAM)&LvItem);

	if(currentEditListViewKey.compare("name")==0){
		AST->setXMLKey(currentTreeViewNodes[currentTreeViewNodeIndex], wchartochar(text).c_str());
		updateTab();
	}
	else AST->setXMLValue(currentTreeViewNodes[currentTreeViewNodeIndex], currentEditListViewKey.c_str(), wchartochar(text).c_str());
}

void QeWindow::updateListView() {
	HTREEITEM hSelectedItem = TreeView_GetSelection(treeViewList);
	if (!hSelectedItem) return;
	
	TCHAR buffer[512];

	TVITEM item;
	item.hItem = hSelectedItem;
	item.mask = TVIF_TEXT;
	item.cchTextMax = 512;
	item.pszText = buffer;

	if (!TreeView_GetItem(treeViewList, &item)) return;
	
	currentTreeViewNodeIndex = (int)item.lParam;
	ListView_DeleteAllItems(listViewDetail);

	if (currentTreeViewNodes.size() <= currentTreeViewNodeIndex) return;

	LVITEM lvi;
	lvi.mask = LVIF_TEXT;
	lvi.iItem = 0;
	lvi.iSubItem = 0;
	lvi.pszText = L"name";
	ListView_InsertItem(listViewDetail, &lvi);
	std::wstring ws;

	for (int i = 0; i< currentTreeViewNodes[currentTreeViewNodeIndex]->eKeys.size() ; ++i) {
		
		lvi.iItem = i+1;
		ws = chartowchar(currentTreeViewNodes[currentTreeViewNodeIndex]->eKeys[i]);
		lvi.pszText = const_cast<LPWSTR>(ws.c_str());
		ListView_InsertItem(listViewDetail, &lvi);
	}
	
	lvi.iItem = 0;
	lvi.iSubItem = 1;
	ws = chartowchar(currentTreeViewNodes[currentTreeViewNodeIndex]->key);
	lvi.pszText = const_cast<LPWSTR>(ws.c_str());
	ListView_SetItem(listViewDetail, &lvi);

	for (int i = 0; i< currentTreeViewNodes[currentTreeViewNodeIndex]->eKeys.size(); ++i) {

		lvi.iItem = i + 1;
		ws = chartowchar(currentTreeViewNodes[currentTreeViewNodeIndex]->eValues[i]);
		lvi.pszText = const_cast<LPWSTR>(ws.c_str());
		ListView_SetItem(listViewDetail, &lvi);
	}
}

void QeWindow::updateTab() {

	currentTreeViewNodes.clear();
	currentTreeViewNodeIndex = INDEX_NONE;
	currentTabIndex = TabCtrl_GetCurSel(tabControlCategory);

	QeAssetXML * node = AST->getXMLNode(1, AST->CONFIG);

	TreeView_DeleteAllItems(treeViewList);
	ListView_DeleteAllItems(listViewDetail);

	node = node->nexts[currentTabIndex];

	for (int i = 0; i< node->nexts.size() ; ++i) {
		addToTreeView(node->nexts[i], TVI_FIRST);
	}
}

void QeWindow::addToTreeView(QeAssetXML * node, HTREEITEM parent ) {

	TVITEM tvi;
	TVINSERTSTRUCT tvins;

	tvi.mask = TVIF_TEXT | TVIF_SELECTEDIMAGE | TVIF_PARAM;

	std::string s = node->key;
	s += " ";
	s += std::to_string(node->nexts.size());
	std::wstring ws = chartowchar(s);

	tvi.pszText = const_cast<LPWSTR>(ws.c_str());
	tvi.cchTextMax = 256;
	tvi.lParam = (LPARAM)currentTreeViewNodes.size();
	currentTreeViewNodes.push_back(node);
	
	tvins.item = tvi;

	//tvins.hInsertAfter = nullptr;

	if (parent == TVI_FIRST)	tvins.hParent = TVI_ROOT;
	else						tvins.hParent = parent;

	HTREEITEM item = TreeView_InsertItem(treeViewList, &tvins);

	for (int i = 0; i< node->nexts.size(); ++i) {
		addToTreeView(node->nexts[i], item);
	}
}

void QeWindow::initialize() {

	if (bInit) return;
	bInit = true;

	if (!DEBUG->isConsole()) FreeConsole();

	windowInstance = GetModuleHandle(nullptr);

	openMainWindow();
	openEditWindow();
}

std::string QeWindow::getWindowTitle(){
	std::string device(VK->deviceProperties.deviceName);
	std::string windowTitle;
	QeAssetXML * node = AST->getXMLNode(3, AST->CONFIG, "setting", "application");
	windowTitle = AST->getXMLValue( node ,1, "applicationName");
	windowTitle.append(" ");
	windowTitle.append(AST->getXMLValue(node, 1, "applicationVersion"));
	windowTitle.append(" - ");
	windowTitle.append(AST->getXMLValue(node, 1, "engineName"));
	windowTitle.append(" ");
	windowTitle.append(AST->getXMLValue(node, 1, "engineVersion"));
	windowTitle.append(" - VulkanAPI ");
	windowTitle.append(AST->getXMLValue(node, 1, "VulkanAPIVersion"));
	windowTitle.append(" - ");
	windowTitle.append(device);
	windowTitle.append(" - ");
	windowTitle.append(std::to_string(QE->currentFPS));
	windowTitle.append("/");
	windowTitle.append(AST->getXMLValue(4, AST->CONFIG, "setting", "environment", "FPS"));
	windowTitle.append(" FPS - ");
	windowTitle.append(SCENE->name);
	windowTitle.append(" ");
	windowTitle.append(std::to_string(SCENE->eid));
	return windowTitle;
}

void QeWindow::update1() {

	std::string windowTitle = getWindowTitle();
	std::wstring ws = chartowchar(windowTitle);
	SetWindowText(mainWindow, ws.c_str());

	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	consoleInput();
}

void QeWindow::update2() {}

void QeWindow::consoleInput() {
	if (!DEBUG->isConsole())	return;

	if(!_kbhit()) return;
	char cur = _getch();

	switch ( cur ) {
	case VK_ESCAPE:
		QE->bClosed = true;
		break;
	case VK_RETURN:
		std::cout << std::endl;
		if (inputData.consoleCommandInput.length()>0)	CMD(inputData.consoleCommandInput);
		inputData.consoleCommandInput.clear();
		break;
	case VK_BACK:
		if (!inputData.consoleCommandInput.empty()) {
			inputData.consoleCommandInput.pop_back();
			std::cout << "\b" << " " << "\b";
		}
		break;
	default:
		inputData.consoleCommandInput += cur;
		std::cout << cur;
		break;
	}

	//hout = GetStdHandle(STD_OUTPUT_HANDLE);
	//initializeConsole(hout);
	/*HANDLE input_handle = GetStdHandle(STD_INPUT_HANDLE);
	INPUT_RECORD input_record;
	DWORD input_size = 1;
	DWORD events = 0;
	PeekConsoleInput(input_handle, &input_record, input_size, &events);
	if (events <= 0) return;

	ReadConsoleInput(input_handle, &input_record, input_size, &events);
	if (input_record.EventType != KEY_EVENT || !input_record.Event.KeyEvent.bKeyDown) return;

	switch (input_record.Event.KeyEvent.wVirtualKeyCode) {
	case VK_ESCAPE:
		QE->bClosed = true;
		break;
	case VK_RETURN:
		std::string str;
		//std::getline(std::cin, str);
		if(str.length()>0)	CMD(str);
		break;
	}*/
}

std::wstring QeWindow::chartowchar(std::string s) {
	wchar_t rtn[4096];
	size_t outSize;
	mbstowcs_s(&outSize, rtn, 4096, s.c_str(), s.length());
	return rtn;
}

std::string QeWindow::wchartochar(std::wstring s) {
	char rtn[4096];
	size_t outSize;
	wcstombs_s(&outSize, rtn, 4096, s.c_str(), s.length());
	return rtn;
}
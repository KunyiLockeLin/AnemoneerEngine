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
	SetFocus(window);
}

void QeWindow::sendCommand() {

	wchar_t  lpString[256];
	GetWindowText(commandBox, lpString, 256);
	CMD(wchartochar(lpString));
}

void QeWindow::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
	case WM_CLOSE:
		QE->bClosed = true;
		break;
	case WM_EXITSIZEMOVE:
		VP->bRecreateRender = true;
		break;

	default:

		switch (wParam) {
		case VK_ESCAPE:
			if(uMsg != WM_IME_COMPOSITION)	QE->bClosed = true;
			break;
		case KEY_FSLASH:
			SetWindowText(commandBox, L"");
			ShowWindow(commandBox, SW_SHOW);
			SetFocus(commandBox);
			break;
		default:
			QE->currentActivity->eventInput(uMsg, int(wParam), LOWORD(lParam), HIWORD(lParam));
			break;
		}

		break;
	}
}


void QeWindow::getWindowSize(int& width, int& height) {

	RECT rect;
	GetClientRect(window, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
}

void QeWindow::resize() {
	RECT windowRect;
	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = std::stoi(AST->getXMLValue(3, AST->CONFIG, "envir", "width"));
	windowRect.bottom = std::stoi(AST->getXMLValue(3, AST->CONFIG, "envir", "height"));

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
	VP->bRecreateRender = true;
}

void QeWindow::init() {

	if (bInit) return;
	bInit = true;

	if (!DEBUG->isConsole()) FreeConsole();

	windowInstance = GetModuleHandle(nullptr);

	WNDCLASSEX wndClass;
	std::wstring title = chartowchar(AST->getXMLValue(2, AST->CONFIG, "applicationName"));

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

	if (!RegisterClassEx(&wndClass)){
		LOG( "Could not register window class!");
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

	window = CreateWindowEx(0, title.c_str(), 0,
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 0, 0, NULL, NULL, windowInstance, NULL);

	//if (!settings.fullscreen){
	// Center on screen
	//uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right) / 2;
	//uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom) / 2;
	//SetWindowPos(window, 0, x, y, 0, 0, SWP_NOSIZE);
	//}
	resize();

	if (!window) {
		LOG( "Could not create window!" );
		fflush(stdout);
		exit(1);
	}

	ShowWindow(window, SW_SHOW);
	SetForegroundWindow(window);
	SetFocus(window);

	int width; int height;
	getWindowSize(width, height);
	commandBox = CreateWindow(L"EDIT", L"", WS_CHILD ,
		0, 0, width, 20, window, (HMENU)1, NULL, NULL);

	ShowWindow(commandBox, SW_HIDE);
	WIN->DefEditProc = (WNDPROC)SetWindowLongPtr(WIN->commandBox, GWLP_WNDPROC, (LONG_PTR)(EditProc));

}

std::string QeWindow::getWindowTitle(){
	std::string device(VK->deviceProperties.deviceName);
	std::string windowTitle;
	windowTitle = AST->getXMLValue(2, AST->CONFIG, "applicationName");
	windowTitle.append(" ");
	windowTitle.append(AST->getXMLValue(2, AST->CONFIG, "applicationVersion"));
	windowTitle.append(" - ");
	windowTitle.append(AST->getXMLValue(2, AST->CONFIG, "engineName"));
	windowTitle.append(" ");
	windowTitle.append(AST->getXMLValue(2, AST->CONFIG, "engineVersion"));
	windowTitle.append(" - ");
	windowTitle.append("VulkanAPI");
	windowTitle.append(" ");
	windowTitle.append(AST->getXMLValue(2, AST->CONFIG, "VulkanAPIVersion"));
	windowTitle.append(" - ");
	windowTitle.append(device);
	windowTitle.append(" - ");
	windowTitle.append(std::to_string(QE->currentFPS));
	windowTitle.append(" fps");

	return windowTitle;
}

void QeWindow::update(float time) {

	std::string windowTitle = getWindowTitle();
	std::wstring ws = chartowchar(windowTitle);
	SetWindowText(window, ws.c_str());

	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	consoleInput();
}

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
		if (consoleCommandInput.length()>0)	CMD(consoleCommandInput);
		consoleCommandInput.clear();
		break;
	case VK_BACK:
		if (!consoleCommandInput.empty()) {
			consoleCommandInput.pop_back();
			std::cout << "\b" << " " << "\b";
		}
		break;
	default:
		consoleCommandInput += cur;
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
	wchar_t rtn[256];
	size_t outSize;
	mbstowcs_s(&outSize, rtn, 256, s.c_str(), s.length());
	return rtn;
}

std::string QeWindow::wchartochar(std::wstring s) {
	char rtn[256];
	size_t outSize;
	wcstombs_s(&outSize, rtn, 256, s.c_str(), s.length());
	return rtn;
}
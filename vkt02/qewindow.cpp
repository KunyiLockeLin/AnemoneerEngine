#include "qewindow.h"

bool QeWindow::isWinodowShouldClose() {
	return bClosed;
}


void QeWindow::getWindowSize(int& width, int& height) {

	RECT rect;
	GetClientRect(window, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	QE->window->handleMessages(hWnd, uMsg, wParam, lParam);
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void QeWindow::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg){
	case WM_CLOSE:
		bClosed = true;
		break;
	case WM_PAINT:
		ValidateRect(window, NULL);
		break;
	case WM_KEYDOWN:
		switch (wParam){
		case VK_ESCAPE:
			bClosed = true;
			break;
		case KEY_W:
		case VK_UP:
			QE->camera->move(QeVector3f(0,0,1));
			break;
		case KEY_S:
		case VK_DOWN:
			QE->camera->move(QeVector3f(0, 0, -1));
			break;
		case KEY_D:
		case VK_RIGHT:
			QE->camera->move(QeVector3f(1, 0, 0));
			break;
		case KEY_A:
		case VK_LEFT:
			QE->camera->move(QeVector3f(-1, 0, 0));
			break;
		case KEY_Q:
			QE->camera->move(QeVector3f(0, 1, 0));
			break;
		case KEY_E:
			QE->camera->move(QeVector3f(0, -1, 0));
			break;
		case KEY_R:
			QE->camera->reset();
			break;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		QE->camera->setMousePos(QeVector2i(LOWORD(lParam), HIWORD(lParam)));
		break;
	case WM_MOUSEMOVE:

		switch (wParam) {
		case MK_LBUTTON:
			QE->camera->rotateTarget(QeVector2i(LOWORD(lParam), HIWORD(lParam)));
			break;
		case MK_RBUTTON:
			QE->camera->rotatePos(QeVector2i(LOWORD(lParam), HIWORD(lParam)));
			break;
		}
		break;
	}
}

void QeWindow::init() {

	windowInstance = GetModuleHandle(nullptr);

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
	wndClass.lpszClassName = convert(AST->getString("title"));
	wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&wndClass))
	{
		std::cout << "Could not register window class!\n";
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

	DWORD dwExStyle;
	DWORD dwStyle;

	/*if (settings.fullscreen){
	dwExStyle = WS_EX_APPWINDOW;
	dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	} else {*/
	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	//}

	RECT windowRect;
	windowRect.left = 0L;
	windowRect.top = 0L;
	windowRect.right = WIDTH;//settings.fullscreen ? (long)screenWidth : (long)width;
	windowRect.bottom = HEIGHT;//settings.fullscreen ? (long)screenHeight : (long)height;

	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	window = CreateWindowEx(0,
		convert(AST->getString("title")),
		convert(AST->getString("title")),
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0,
		0,
		windowRect.right- windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		windowInstance,
		NULL);

	//if (!settings.fullscreen){
	// Center on screen
	uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right) / 2;
	uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom) / 2;
	SetWindowPos(window, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	//}

	if (!window) {
		std::cout << "Could not create window!\n";
		fflush(stdout);
		exit(1);
	}

	ShowWindow(window, SW_SHOW);
	SetForegroundWindow(window);
	SetFocus(window);
}

std::string QeWindow::getWindowTitle()
{
	std::string device(QE->deviceProperties.deviceName);
	std::string windowTitle;
	windowTitle = AST->getString("title");
	windowTitle.append(" - ");
	windowTitle.append(device);
	windowTitle.append(" - ");
	windowTitle.append(std::to_string(QE->currentFPS));
	windowTitle.append(" fps");

	return windowTitle;
}

void QeWindow::initSurface() {

	VkResult err = VK_SUCCESS;

	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hinstance = windowInstance;
	surfaceCreateInfo.hwnd = window;
	err = vkCreateWin32SurfaceKHR(QE->instance, &surfaceCreateInfo, nullptr, &surface);

	if (err != VK_SUCCESS) {
		std::cout << "Could not create surface!\n";
	}

	bClosed = false;
}

void QeWindow::cleanup() {
	vkDestroySurfaceKHR(QE->instance, surface, nullptr);
}


void QeWindow::update(float time) {

	MSG msg;
	std::string windowTitle = getWindowTitle();
	SetWindowText(window, convert(windowTitle));
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT) {
			bClosed = true;
			break;
		}
	}
}


TCHAR* QeWindow::convert(std::string _s) {

	size_t size = _s.size();
	TCHAR *rtn = new TCHAR[size + 1];
	rtn[size] = 0;
	for (size_t i = 0; i < size; ++i)
		rtn[i] = _s[i];
	return rtn;
}
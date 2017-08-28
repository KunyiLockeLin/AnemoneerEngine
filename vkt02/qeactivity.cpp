#include "qeactivity.h"


void QeActivity::init() {

	light = OBJMGR->getLight();

	model = OBJMGR->getModel("model", 0);
	model->setPosition(QeVector3f(0, 2, 0));

	model1 = OBJMGR->getModel("model", 1);
	model1->setPosition(QeVector3f(0, -3, 0));
}

void QeActivity::eventInput(int _input1, int _input2, int _param1, int _param2) {
	
	switch (_input1) {

	case WM_KEYDOWN:
		switch (_input2) {
		case VK_ESCAPE:
			WIN->bClosed = true;
			break;
		case VK_NUMPAD1:
		case VK_NUMPAD2:
		case VK_NUMPAD3:
		case VK_NUMPAD4:
		case VK_NUMPAD5:
		case VK_NUMPAD6:
		case VK_NUMPAD7:
		case VK_NUMPAD8:
		case VK_NUMPAD9:
			VP->setTargetCamera(_input2 - VK_NUMPAD0);
			break;
		case VK_ADD:
			VP->addNewViewport();
			break;
		case VK_SUBTRACT:
			VP->popViewport();
			break;
		case KEY_W:
		case VK_UP:
			VP->getTargetCamera()->move(QeVector3f(0, 0, 1));
			break;
		case KEY_S:
		case VK_DOWN:
			VP->getTargetCamera()->move(QeVector3f(0, 0, -1));
			break;
		case KEY_D:
		case VK_RIGHT:
			VP->getTargetCamera()->move(QeVector3f(1, 0, 0));
			break;
		case KEY_A:
		case VK_LEFT:
			VP->getTargetCamera()->move(QeVector3f(-1, 0, 0));
			break;
		case KEY_Q:
			VP->getTargetCamera()->move(QeVector3f(0, 1, 0));
			break;
		case KEY_E:
			VP->getTargetCamera()->move(QeVector3f(0, -1, 0));
			break;
		case KEY_R:
			VP->getTargetCamera()->init();
			break;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		VP->getTargetCamera()->setMousePos(QeVector2i(_param1, _param2));
		break;
	case WM_MOUSEMOVE:

		switch (_input2) {
		case MK_LBUTTON:
			VP->getTargetCamera()->rotateTarget(QeVector2i(_param1, _param2));
			break;
		case MK_RBUTTON:
			VP->getTargetCamera()->rotatePos(QeVector2i(_param1, _param2));
			break;
		}
		break;
	}
}

void QeActivity::update(float time) {
	OBJMGR->update(time);
}
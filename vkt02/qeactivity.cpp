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
		case KEY_W:
		case VK_UP:
			VP->cameras[0]->move(QeVector3f(0, 0, 1));
			break;
		case KEY_S:
		case VK_DOWN:
			VP->cameras[0]->move(QeVector3f(0, 0, -1));
			break;
		case KEY_D:
		case VK_RIGHT:
			VP->cameras[0]->move(QeVector3f(1, 0, 0));
			break;
		case KEY_A:
		case VK_LEFT:
			VP->cameras[0]->move(QeVector3f(-1, 0, 0));
			break;
		case KEY_Q:
			VP->cameras[0]->move(QeVector3f(0, 1, 0));
			break;
		case KEY_E:
			VP->cameras[0]->move(QeVector3f(0, -1, 0));
			break;
		case KEY_R:
			VP->cameras[0]->init();
			break;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		VP->cameras[0]->setMousePos(QeVector2i(_param1, _param2));
		break;
	case WM_MOUSEMOVE:

		switch (_input2) {
		case MK_LBUTTON:
			VP->cameras[0]->rotateTarget(QeVector2i(_param1, _param2));
			break;
		case MK_RBUTTON:
			VP->cameras[0]->rotatePos(QeVector2i(_param1, _param2));
			break;
		}
		break;
	}
}

void QeActivity::update(float time) {
	OBJMGR->update(time);
}
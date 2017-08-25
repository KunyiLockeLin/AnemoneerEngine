#include "qeactivity.h"


void QeActivity::init() {

	camera = OBJMGR->getCamera();
	light = OBJMGR->getLight();

	model = OBJMGR->getModel("model", 0);
	model->setPosition(QeVector3f(0, 2, 0));

	model1 = OBJMGR->getModel("model", 1);
	model1->setPosition(QeVector3f(0, -3, 0));
}

void QeActivity::eventInput(int _input1, int _input2, int _mouseX, int _mouseY) {

	switch (_input1) {
	case WM_CLOSE:
		WIN->bClosed = true;
		break;
	case WM_KEYDOWN:
		switch (_input2) {
		case VK_ESCAPE:
			WIN->bClosed = true;
			break;
		case KEY_W:
		case VK_UP:
			camera->move(QeVector3f(0, 0, 1));
			break;
		case KEY_S:
		case VK_DOWN:
			camera->move(QeVector3f(0, 0, -1));
			break;
		case KEY_D:
		case VK_RIGHT:
			camera->move(QeVector3f(1, 0, 0));
			break;
		case KEY_A:
		case VK_LEFT:
			camera->move(QeVector3f(-1, 0, 0));
			break;
		case KEY_Q:
			camera->move(QeVector3f(0, 1, 0));
			break;
		case KEY_E:
			camera->move(QeVector3f(0, -1, 0));
			break;
		case KEY_R:
			camera->init();
			break;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		camera->setMousePos(QeVector2i(_mouseX, _mouseY));
		break;
	case WM_MOUSEMOVE:

		switch (_input2) {
		case MK_LBUTTON:
			camera->rotateTarget(QeVector2i(_mouseX, _mouseY));
			break;
		case MK_RBUTTON:
			camera->rotatePos(QeVector2i(_mouseX, _mouseY));
			break;
		}
		break;
	}
}

void QeActivity::update(float time) {
	OBJMGR->update(time);
}
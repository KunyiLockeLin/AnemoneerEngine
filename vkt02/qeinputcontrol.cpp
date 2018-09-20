#include "qeheader.h"


void QeInputControl::initialize(QeAssetXML* _property, QeObject* _owner) {
	QeComponent::initialize(_property, _owner);
	WIN->inputControls.push_back(this);
}

void QeInputControl::clear() {
	eraseElementFromVector(WIN->inputControls, this);
}

void QeInputControl::updateInput() {

	switch (WIN->inputData.inputType) {

// viewport
	case WM_KEYDOWN:
		switch (WIN->inputData.inputKey) {

		case VK_NUMPAD1:
		case VK_NUMPAD2:
		case VK_NUMPAD3:
		case VK_NUMPAD4:
		case VK_NUMPAD5:
		case VK_NUMPAD6:
		case VK_NUMPAD7:
		case VK_NUMPAD8:
		case VK_NUMPAD9:
			GRAP->setTargetCamera(WIN->inputData.inputKey - VK_NUMPAD1);
			break;
		case VK_ADD:
			GRAP->addNewViewport(eRender_main);
			break;
		case VK_SUBTRACT:
			GRAP->popViewport(eRender_main);
			break;

// camera
		case VK_UP:
			GRAP->getTargetCamera()->rotateTarget({ -10.f, 0.f });
			break;
		case VK_DOWN:
			GRAP->getTargetCamera()->rotateTarget({ 10.f, 0.f });
			break;
		case VK_RIGHT:
			GRAP->getTargetCamera()->rotateTarget({ 0.f, 10.f });
			break;
		case VK_LEFT:
			GRAP->getTargetCamera()->rotateTarget({ 0.f, -10.f });
			break;

		case KEY_E:
			GRAP->getTargetCamera()->move({ 0.f, 0.2f, 0.f });
			break;
		case KEY_C:
			GRAP->getTargetCamera()->move({ 0.f, -0.2f, 0.f });
			break;
		case KEY_A:
			GRAP->getTargetCamera()->move({ -1.f, 0.f, 0.f });
			break;
		case KEY_D:
			GRAP->getTargetCamera()->move({ 1.f, 0.f, 0.f });
			break;
		case KEY_W:
			GRAP->getTargetCamera()->move({ 0.f, 0.f, 0.5f });
			break;
		case KEY_S:
			GRAP->getTargetCamera()->move({ 0.f, 0.f, -0.5f });
			break;
		case KEY_Q:
			GRAP->getTargetCamera()->move({ 0.f, 0.f, 1.f }, false);
			break;
		case KEY_Z:
			GRAP->getTargetCamera()->move({ 0.f, 0.f, -1.f }, false);
			break;
		case KEY_X:
			GRAP->getTargetCamera()->reset();
			break;
		default:
			break;
		}
		break;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		GRAP->getTargetCamera()->setMousePos(WIN->inputData.mousePos);
		break;
	case WM_MOUSEMOVE:

		switch (WIN->inputData.inputKey) {
		case MK_LBUTTON:
			GRAP->getTargetCamera()->rotateTargetByMouse(WIN->inputData.mousePos);
			break;
		case MK_RBUTTON:
			//rotatePos(inputData.mousePos);
			GRAP->getTargetCamera()->zoomInOut(WIN->inputData.mousePos);
			break;
		}
		break;
	default:
		break;
	}
}
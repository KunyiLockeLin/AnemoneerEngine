#pragma once

#include "qeheader.h"

class QeActivity
{
public:
	QeActivity(QeObjectMangerKey& _key) {}
	~QeActivity() {}

	QeModel*	model;
	QeModel*	model1;
	QeCamera*	camera;
	QeLight*	light;

	void init();
	void eventInput( int _input1, int _input2, int _mouseX, int _mouseY );
	void update(float time);
};


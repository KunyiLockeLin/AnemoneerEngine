#pragma once

#include "qeheader.h"

class QeActivity
{
public:
	QeActivity(QeObjectMangerKey& _key) {}
	~QeActivity() {}

	QeLight*	light;

	void init();
	void eventInput( int _input1, int _input2, int _param1, int _param2);
	void update(float time);
};


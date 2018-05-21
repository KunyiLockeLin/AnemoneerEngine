#include "qeheader.h"


void QeCommand::inputCommand(std::string& command) {

	if (command.empty()) return;

	std::vector<std::string> res;
	std::string result;
	std::stringstream input(command);
	while (input >> result)
		res.push_back(result);

	if (res[0].compare("restart") == 0) {
		QE->bRestart = true;
		if (res.size() > 1)	QE->activityName = res[1];
	}
	else if (res[0].compare("resetcamera") == 0) {
		VP->getTargetCamera()->reset();
		//if(res.size() >1)	VP->getTargetCamera()->type = QeCameraType(atoi(res[1].c_str()));
	}
	else if (res[0].compare("showmesh") == 0) {
		VK->bShowMesh = !VK->bShowMesh;
		VP->bRecreateRender = true;
	}
}


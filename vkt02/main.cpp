#include "qeheader.h"


int main() {
	
	try {
		while (1) {
			QE->run();
			if (!QE->bRestart)	break;
			else				GLB.restart();
		}
	}
	catch (const std::runtime_error& e) {
		LOG(e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
#include "qeheader.h"


int main() {
	
	while (1) {
		QE->run();
		if (!QE->bRestart)	break;
		else				GLB.restart();
	}

	return EXIT_SUCCESS;
}
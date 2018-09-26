#include "qeheader.h"


int main() {
	
	try {
		QE->run();
	}
	catch (const std::runtime_error& e) {
		LOG( e.what() );
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
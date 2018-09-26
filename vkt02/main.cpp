#include "qeheader.h"


int main() {
	
	while (1) {
		try {
			QE->run();
		}
		catch (const std::runtime_error& e) {
			LOG( e.what() );
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}
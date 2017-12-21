#include "qeheader.h"



void QueenEngine::run() {
	lastTime = std::chrono::high_resolution_clock::now();

	VLK->init();
	VP->init();
	
	activity = OBJMGR->getActivity(0);
	activity->init();

	mainLoop();
	cleanup();
}


void QueenEngine::mainLoop() {
	while (!WIN->isWinodowShouldClose()) {

		std::chrono::steady_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count() / 1000.0f;

		if (time >= 1.0f / std::stoi(AST->getXMLValue(3, AST->CONFIG, "envir", "FPS"))) {

			lastTime = currentTime;
			currentFPS = int(1 / time);
			
			activity->update(time);
			VP->update(time);
			VLK->update(time);
			WIN->update(time);
		}
	}
	VLK->deviceWaitIdle();
}

void QueenEngine::cleanup() {
	//cleanupSwapChain();

	//vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	//vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
	//vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
	//vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
	//vkDestroyCommandPool(device, commandPool, nullptr);
	//vkDestroyDevice(device, nullptr);
	//DestroyDebugReportCallbackEXT(instance, callback, nullptr);
	//WIN->cleanup();
	//vkDestroyInstance(instance, nullptr);
}
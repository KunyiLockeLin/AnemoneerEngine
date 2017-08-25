#include "queenengine.h"



void QueenEngine::run() {
	AST->loadConfig();
	lastTime = std::chrono::high_resolution_clock::now();

	WIN->init();
	VLK->init();

	viewport = new QeViewport();
	viewport->init();

	activity = OBJMGR->getActivity();
	activity->init();

	mainLoop();
	cleanup();
}


void QueenEngine::mainLoop() {
	while (!WIN->isWinodowShouldClose()) {

		std::chrono::steady_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count() / 1000.0f;

		if (time >= 1.0f / std::stoi(AST->getString("FPS"))) {

			lastTime = currentTime;
			currentFPS = int(1 / time);
			
			activity->update(time);
			viewport->update(time);
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
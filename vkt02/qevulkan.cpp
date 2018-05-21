#include "qeheader.h"


QeVKBuffer::~QeVKBuffer(){
	if (buffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(VK->device, buffer, nullptr);
		buffer = VK_NULL_HANDLE;
	}
	if (memory != VK_NULL_HANDLE) {
		vkFreeMemory(VK->device, memory, nullptr);
		memory = VK_NULL_HANDLE;
	}
}

QeVKImageBuffer::~QeVKImageBuffer(){
	if (image != VK_NULL_HANDLE) {
		vkDestroyImage(VK->device, image, nullptr);
		image = VK_NULL_HANDLE;
	}
	if (memory != VK_NULL_HANDLE){
		vkFreeMemory(VK->device, memory, nullptr);
		memory = VK_NULL_HANDLE;
	}
	if (view != VK_NULL_HANDLE) {
		vkDestroyImageView(VK->device, view, nullptr);
		view = VK_NULL_HANDLE;
	}
}

QeDataDescriptorSet::QeDataDescriptorSet() {
	uboSize = sizeof(QeUniformBufferObject);
	lightSize = sizeof(QeDataLight);
	materialSize = sizeof(QeDataMaterial);
}

 QeVulkan::~QeVulkan() {

	vkDestroySurfaceKHR(VK->instance, surface, nullptr);
	surface = VK_NULL_HANDLE;
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	pipelineLayout = VK_NULL_HANDLE;
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
	descriptorSetLayout = VK_NULL_HANDLE;
	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	descriptorPool = VK_NULL_HANDLE;
	vkDestroyCommandPool(device, commandPool, nullptr);
	commandPool = VK_NULL_HANDLE;

	vkDestroyDevice(device, nullptr);
	device = VK_NULL_HANDLE;
	DestroyDebugReportCallbackEXT(instance, callback, nullptr);
	callback = VK_NULL_HANDLE;
	vkDestroyInstance(instance, nullptr);
	instance = VK_NULL_HANDLE;
}

void QeVulkan::init() {

	if (bInit) return;
	bInit = true;

	createInstance();
	setupDebugCallback();
	surface = createSurface(WIN->window, WIN->windowInstance);
	pickPhysicalDevice();
	createLogicalDevice();
	createDescriptorPool();
	descriptorSetLayout = createDescriptorSetLayout();
	pipelineLayout = createPipelineLayout( descriptorSetLayout);
	createCommandPool();
}

void QeVulkan::updateRender(float time) {}
void QeVulkan::updateCompute(float time) {}

VkResult QeVulkan::CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) {
	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	
	if (func != nullptr)	return func(instance, pCreateInfo, pAllocator, pCallback);
	else					return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void QeVulkan::DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	
	if (func != nullptr)	func(instance, callback, pAllocator);
}

void QeVulkan::createInstance() {
	if (DEBUG->isDebug() && !checkValidationLayerSupport())	LOG("validation layers requested, but not available!");
	
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = AST->getXMLValue(2, AST->CONFIG, "applicationName");

	std::vector<std::string> vs = ENCODE->split(AST->getXMLValue(2, AST->CONFIG, "applicationVersion"), ".");
	appInfo.applicationVersion = VK_MAKE_VERSION(atoi(vs[0].c_str()), atoi(vs[1].c_str()), atoi(vs[2].c_str()));
	
	appInfo.pEngineName = AST->getXMLValue(2, AST->CONFIG, "engineName");
	vs = ENCODE->split(AST->getXMLValue(2, AST->CONFIG, "engineVersion"), ".");
	appInfo.engineVersion = VK_MAKE_VERSION(atoi(vs[0].c_str()), atoi(vs[1].c_str()), atoi(vs[2].c_str()));

	vs = ENCODE->split(AST->getXMLValue(2, AST->CONFIG, "VulkanAPIVersion"), ".");
	appInfo.apiVersion = VK_MAKE_VERSION(atoi(vs[0].c_str()), atoi(vs[1].c_str()), atoi(vs[2].c_str()));

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;
	//createInfo.enabledExtensionCount = 0;
	//createInfo.ppEnabledExtensionNames = 0;

	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (DEBUG->isDebug()) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else	createInfo.enabledLayerCount = 0;

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)	LOG("failed to create instance!");
}

void QeVulkan::setupDebugCallback() {
	if (!DEBUG->isDebug()) return;

	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;

	if (CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback) != VK_SUCCESS)	LOG("failed to set up debug callback!");
}

void QeVulkan::pickPhysicalDevice() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0)	LOG("failed to find GPUs with Vulkan support!");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
	}}

	if (physicalDevice == VK_NULL_HANDLE)	LOG("failed to find a suitable GPU!");

	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
	//vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
	//vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);
}

void QeVulkan::createLogicalDevice() {
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);

	std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily, indices.computeFamily };
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	float queuePriority = 1.0f;
	for (int queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.pNext = nullptr;
		queueCreateInfo.flags = 0;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.fillModeNonSolid = VK_TRUE;
	deviceFeatures.multiViewport = VK_TRUE;
	deviceFeatures.geometryShader = VK_TRUE;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;
	//createInfo.enabledExtensionCount = 0;
	//createInfo.ppEnabledExtensionNames = 0;
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (DEBUG->isDebug()) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else	createInfo.enabledLayerCount = 0;

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)	LOG("failed to create logical device!");

	vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily, 0, &presentQueue);
	vkGetDeviceQueue(device, indices.computeFamily, 0, &computeQueue);
}

void QeVulkan::createSwapChain(VkSurfaceKHR& surface, VkSwapchainKHR& swapChain, VkExtent2D& swapChainExtent, VkFormat& swapChainImageFormat, std::vector<VkImage>& swapChainImages, std::vector<VkImageView>& swapChainImageViews) {
	
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		imageCount = swapChainSupport.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT| VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
	uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily, (uint32_t)indices.computeFamily };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	//createInfo.oldSwapchain = oldSwapchain;

	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)	LOG("failed to create swap chain!");

	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;

	swapChainImageViews.resize(swapChainImages.size());

	for (uint32_t i = 0; i < swapChainImages.size(); i++) {
		swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

VkRenderPass QeVulkan::createRenderPass(VkFormat& swapChainImageFormat) {
	VkAttachmentDescription colorAttachment1 = {};
	//colorAttachment1.flags = 0;
	colorAttachment1.flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
	colorAttachment1.format = swapChainImageFormat;
	colorAttachment1.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment1.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment1.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment1.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment1.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment1.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment1.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkAttachmentDescription colorAttachment2 = {};
	//colorAttachment2.flags = 0;
	colorAttachment2.flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
	colorAttachment2.format = swapChainImageFormat;
	colorAttachment2.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment2.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment2.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment2.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment2.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment2.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment2.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription depthAttachment = {};
	//depthAttachment.flags = 0;
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef1 = {};
	colorAttachmentRef1.attachment = 0;
	colorAttachmentRef1.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference inputAttachmentRef = {};
	inputAttachmentRef.attachment = 0;
	inputAttachmentRef.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkAttachmentReference colorAttachmentRef2 = {};
	colorAttachmentRef2.attachment = 2;
	colorAttachmentRef2.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass1 = {};
	subpass1.flags = 0;
	subpass1.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass1.colorAttachmentCount = 1;
	subpass1.pColorAttachments = &colorAttachmentRef1;
	subpass1.pDepthStencilAttachment = &depthAttachmentRef;
	
	VkSubpassDescription subpass2 = {};
	subpass2.flags = 0;
	subpass2.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass2.inputAttachmentCount = 1;
	subpass2.pInputAttachments = &inputAttachmentRef;
	subpass2.colorAttachmentCount = 1;
	subpass2.pColorAttachments = &colorAttachmentRef2;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = 0;
	dependency.dstSubpass = 1;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
	dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	std::array<VkAttachmentDescription, 3> attachments = { colorAttachment1, depthAttachment, colorAttachment2 };
	std::array<VkSubpassDescription, 2> subpasses = { subpass1, subpass2 };

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = static_cast<uint32_t>(subpasses.size());;
	renderPassInfo.pSubpasses = subpasses.data();
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VkRenderPass renderPass;
	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)	LOG("failed to create render pass!");
	return renderPass;
}

VkDescriptorSetLayout QeVulkan::createDescriptorSetLayout() {
	
	std::vector<VkDescriptorSetLayoutBinding> bindings;
	bindings.resize(descriptorSetBufferNumber + descriptorSetImageNumber + descriptorSetInputAttachmentNumber);

	int i = 0;
	int sum = descriptorSetBufferNumber;
	for ( i = 0; i<sum;++i ) {
		bindings[i].binding = i;
		bindings[i].descriptorCount = 1;
		bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		bindings[i].pImmutableSamplers = nullptr;
		bindings[i].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT |VK_SHADER_STAGE_FRAGMENT_BIT;
	}
	i = sum;
	sum += descriptorSetImageNumber;
	for ( ; i<sum; ++i) {
		bindings[i].binding = i;
		bindings[i].descriptorCount = 1;
		bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		bindings[i].pImmutableSamplers = nullptr;
		bindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	}
	i = sum;
	sum += descriptorSetInputAttachmentNumber;
	for (; i < sum; ++i) {
		bindings[i].binding = i;
		bindings[i].descriptorCount = 1;
		bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		bindings[i].pImmutableSamplers = nullptr;
		bindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	}
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	VkDescriptorSetLayout descriptorSetLayout;
	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)	LOG("failed to create descriptor set layout!");

	return descriptorSetLayout;
}

VkPipelineLayout QeVulkan::createPipelineLayout( VkDescriptorSetLayout& descriptorSetLayout) {

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

	VkPipelineLayout pipelineLayout;
	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)	LOG("failed to create pipeline layout!");

	return pipelineLayout;
}

void QeVulkan::createFramebuffers(std::vector<VkFramebuffer>& framebuffers, QeVKImageBuffer& sceneImage, 
								QeVKImageBuffer& depthImage, std::vector<VkImageView>& swapChainImageViews, VkExtent2D& swapChainExtent, VkRenderPass& renderPass) {
	framebuffers.resize(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		std::array<VkImageView, 3> attachments = {
			//swapChainImageViews[i],
			sceneImage.view,
			depthImage.view,
			swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS)	LOG("failed to create framebuffer!");
	}
}

void QeVulkan::createCommandPool() {
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice, surface);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.pNext = nullptr;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)	LOG("failed to create graphics command pool!");
}

void QeVulkan::createSceneDepthImage(QeVKImageBuffer& sceneImage, QeVKImageBuffer& depthImage, VkExtent2D& swapChainExtent) {
	VkFormat depthFormat = findDepthFormat();

	createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, 
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage.image, depthImage.memory);

	depthImage.view = createImageView(depthImage.image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	transitionImageLayout(depthImage.image, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	int imageSize = swapChainExtent.width*swapChainExtent.height * 4;
	unsigned char *data = new unsigned char[imageSize];
	memset(data, 0, imageSize);
	createImageData(data, VK_FORMAT_B8G8R8A8_UNORM, imageSize, swapChainExtent.width, swapChainExtent.height, sceneImage.image, sceneImage.memory);
	sceneImage.view = createImageView(sceneImage.image, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
}

VkFormat QeVulkan::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	LOG("failed to find supported format!");
	return VK_FORMAT_UNDEFINED;
}

VkFormat QeVulkan::findDepthFormat() {
	return findSupportedFormat(
	{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

bool QeVulkan::hasStencilComponent(VkFormat format) {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkImageView QeVulkan::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, bool bCubemap) {
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = bCubemap ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

	VkImageView imageView;
	if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) LOG("failed to create texture image view!");

	return imageView;
}

void QeVulkan::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, bool bCubemap) {

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.flags = bCubemap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = bCubemap ? 6 : 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;// VK_IMAGE_LAYOUT_PREINITIALIZED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) LOG("failed to create image!");
	
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) LOG("failed to allocate image memory!");

	vkBindImageMemory(device, image, imageMemory, 0);
}

void QeVulkan::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
	
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (hasStencilComponent(format))	barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	else	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED) {
		barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_HOST_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED) {
		barrier.srcAccessMask = 0;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED ) {
		barrier.srcAccessMask = 0;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	}
	else	LOG("unsupported layout transition!");

	if ( newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if ( newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if ( newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if ( newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else	LOG("unsupported layout transition!");

	vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier );

	endSingleTimeCommands(commandBuffer);
}

void QeVulkan::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, int layer) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = layer;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { width, height, 1 };

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	endSingleTimeCommands(commandBuffer);
}

void QeVulkan::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) LOG("failed to create buffer!");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) LOG("failed to allocate buffer memory!");

	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

VkCommandBuffer QeVulkan::beginSingleTimeCommands() {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void QeVulkan::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void QeVulkan::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}

uint32_t QeVulkan::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	LOG("failed to find suitable memory type!");
	return VK_NULL_HANDLE;
}

void QeVulkan::createSemaphores(VkSemaphore& imageAvailableSemaphore, VkSemaphore& renderFinishedSemaphore) {
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS)
		LOG("failed to create semaphores!");
}

VkSurfaceFormatKHR QeVulkan::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
		return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return availableFormats[0];
}

VkPresentModeKHR QeVulkan::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)		return availablePresentMode;
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) bestMode = availablePresentMode;
	}
	return bestMode;
}

VkExtent2D QeVulkan::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		int width = 0, height = 0;
		WIN->getWindowSize(width, height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

SwapChainSupportDetails QeVulkan::querySwapChainSupport(VkPhysicalDevice& device, VkSurfaceKHR& surface) {
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

bool QeVulkan::isDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices indices = findQueueFamilies(device, surface);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	//bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
		//swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.isComplete() && extensionsSupported && supportedFeatures.samplerAnisotropy;
}

bool QeVulkan::checkDeviceExtensionSupport(VkPhysicalDevice device) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

QueueFamilyIndices QeVulkan::findQueueFamilies(VkPhysicalDevice& device, VkSurfaceKHR& surface) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
			indices.computeFamily = i;
		}

		VkBool32 presentSupport = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport)	indices.presentFamily = i;
		if (indices.isComplete())	break;

		i++;
	}

	return indices;
}

std::vector<const char*> QeVulkan::getRequiredExtensions() {

	std::vector<const char*> extensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };

	if (DEBUG->isDebug()) {
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	return extensions;
}

bool QeVulkan::checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}
		if (!layerFound)	return false;
	}

	return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL QeVulkan::debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData) {
	LOG("validation layer: " + msg );
	return VK_FALSE;
}

void QeVulkan::createDrawCommandBuffers(std::vector<VkCommandBuffer>& drawCommandBuffers, size_t size) {
	drawCommandBuffers.resize(size);

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)drawCommandBuffers.size();

	if (vkAllocateCommandBuffers(device, &allocInfo, drawCommandBuffers.data()) != VK_SUCCESS) LOG("failed to allocate command buffers!");
}

VkSurfaceKHR QeVulkan::createSurface(HWND& window, HINSTANCE& windowInstance) {
	VkResult err = VK_SUCCESS;
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = nullptr;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.hinstance = windowInstance;
	surfaceCreateInfo.hwnd = window;

	VkSurfaceKHR surface;
	err = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);

	if (err != VK_SUCCESS) LOG("Could not create surface!");
	return surface;
}

VkDescriptorSet QeVulkan::createDescriptorSet(VkDescriptorSetLayout& descriptorSetLayout) {
	VkDescriptorSetLayout layouts[] = { descriptorSetLayout };
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts;

	VkDescriptorSet descriptorSet;
	if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet) != VK_SUCCESS) LOG("failed to allocate descriptor set!");

	return descriptorSet;
}

void QeVulkan::setMemory(VkDeviceMemory& memory, void* data, VkDeviceSize size) {

	void* buf;
	vkMapMemory(device, memory, 0, size, 0, &buf);
	memcpy(buf, data, size);
	vkUnmapMemory(device, memory);
}


void QeVulkan::createDescriptorPool() {
	std::array<VkDescriptorPoolSize, 3> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = MAX_DESCRIPTOR_UNIFORM_NUM;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = MAX_DESCRIPTOR_SAMPLER_NUM;
	poolSizes[2].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	poolSizes[2].descriptorCount = MAX_DESCRIPTOR_INPUTATTACH_NUM;
	

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = MAX_DESCRIPTOR_NUM;

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) LOG("failed to create descriptor pool!");
}

VkPipeline QeVulkan::createPipeline(VkShaderModule* vertShader, VkShaderModule* geomShader, VkShaderModule* fragShader, VkBool32 bLine, VkBool32 bAlpha, VkBool32 bDepthTest, VkBool32 bVetex, uint8_t subpassIndex) {

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

	if (vertShader != nullptr) {
		VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = *vertShader;
		vertShaderStageInfo.pName = "main";
		shaderStages.push_back(vertShaderStageInfo);
	}

	if (geomShader != nullptr) {
		VkPipelineShaderStageCreateInfo geomShaderStageInfo = {};
		geomShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		geomShaderStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
		geomShaderStageInfo.module = *geomShader;
		geomShaderStageInfo.pName = "main";
		shaderStages.push_back(geomShaderStageInfo);
	}
	if (fragShader != nullptr) {
		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = *fragShader;
		fragShaderStageInfo.pName = "main";
		shaderStages.push_back(fragShaderStageInfo);
	}

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	if (bVetex) {
		std::string s = "";  // Magic line!!!! In release mode, if it's removed, attributeDescriptions would become bad value and shutdown.
		VkVertexInputBindingDescription bindingDescription = QeVertex::getBindingDescription();
		std::array<VkVertexInputAttributeDescription, 7> attributeDescriptions = QeVertex::getAttributeDescriptions();
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	}
	else {
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;
	}
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = bLine? VK_PRIMITIVE_TOPOLOGY_LINE_LIST: VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = bShowMesh? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	
	rasterizer.cullMode = bVetex ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_FRONT_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

	if (bDepthTest) {
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
	}
	else {
		depthStencil.depthTestEnable = VK_FALSE;
		depthStencil.depthWriteEnable = VK_FALSE;
	}
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	
	if (bAlpha) {
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	}
	else {
		colorBlendAttachment.blendEnable = VK_FALSE;
	}
	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 3;
	dynamicState.pDynamicStates = dynamicStates;

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = (uint32_t)shaderStages.size();
	pipelineInfo.pStages = shaderStages.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &VP->viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = VP->renderPass;
	pipelineInfo.subpass = subpassIndex;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	VkPipeline graphicsPipeline;
	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) LOG("failed to create graphics pipeline!");
	
	return graphicsPipeline;
}


void QeVulkan::updateDescriptorSet(QeDataDescriptorSet& data,  VkDescriptorSet& descriptorSet) {

	uint64_t i = 0;
	uint8_t index = 0;
	uint8_t* pos = (uint8_t*)&data;
	std::vector<uint8_t>	 bindID;
	std::vector<VkBuffer>	 buffers;
	std::vector<uint64_t>	 bufferSizes;
	std::vector<VkImageView> imageViews;
	std::vector<VkSampler>	 samplers;
	std::vector<VkImageView> attachImageViews;

	uint64_t sizeBuffer = sizeof(VkBuffer) + sizeof(uint64_t);
	uint64_t sizeImgae = sizeof(VkImageView) + sizeof(VkSampler);
	uint64_t sizeAttach = sizeof(VkImageView);

	for (i = 0;i<descriptorSetBufferNumber;++i,++index) {
		if ( (*(VkBuffer*)(pos)) != VK_NULL_HANDLE) {
			bindID.push_back( index );
			buffers.push_back(*(VkBuffer*)(pos));
			bufferSizes.push_back(*(uint64_t*)(pos+ sizeof(VkBuffer)));
		}
		pos += sizeBuffer;
	}
	for (i = 0; i<descriptorSetImageNumber; ++i, ++index) {
		if ((*(VkImageView*)(pos)) != VK_NULL_HANDLE) {
			bindID.push_back(index);
			imageViews.push_back(*(VkImageView*)(pos));
			samplers.push_back(*(VkSampler*)(pos + sizeof(VkSampler)));
		}
		pos += sizeImgae;
	}
	for (i = 0; i<descriptorSetInputAttachmentNumber; ++i,++index) {
		if ((*(VkImageView*)(pos)) != VK_NULL_HANDLE) {
			bindID.push_back(index);
			attachImageViews.push_back(*(VkImageView*)(pos));
		}
		pos += sizeAttach;
	}

	uint64_t bufNum = buffers.size();
	uint64_t imageNum = imageViews.size();
	uint64_t attachNum = attachImageViews.size();
	std::vector<VkWriteDescriptorSet> descriptorWrites;
	descriptorWrites.resize(bufNum + imageNum + attachNum);

	std::vector<VkDescriptorBufferInfo> bufInfos;
	bufInfos.resize(bufNum);
	i = 0;
	index = 0;
	for (i = 0; i < bufNum; ++i,++index) {

		bufInfos[i].buffer = buffers[i];
		bufInfos[i].offset = 0;
		bufInfos[i].range = bufferSizes[i];

		descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[index].pNext = nullptr;
		descriptorWrites[index].dstSet = descriptorSet;
		descriptorWrites[index].dstBinding = bindID[index];
		descriptorWrites[index].dstArrayElement = 0;
		descriptorWrites[index].descriptorCount = 1;
		descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[index].pImageInfo = nullptr;
		descriptorWrites[index].pBufferInfo = &bufInfos[i];
		descriptorWrites[index].pTexelBufferView = nullptr;
	}
	
	std::vector<VkDescriptorImageInfo> imgInfos;
	imgInfos.resize(imageNum);
	for (i = 0; i < imageNum; ++i, ++index) {

		imgInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imgInfos[i].sampler = samplers[i];
		imgInfos[i].imageView = imageViews[i];
		
		descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[index].pNext = nullptr;
		descriptorWrites[index].dstSet = descriptorSet;
		descriptorWrites[index].dstBinding = bindID[index];
		descriptorWrites[index].dstArrayElement = 0;
		descriptorWrites[index].descriptorCount = 1;
		descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[index].pImageInfo = &imgInfos[i];
		descriptorWrites[index].pBufferInfo = nullptr;
		descriptorWrites[index].pTexelBufferView = nullptr;
	}

	std::vector<VkDescriptorImageInfo> attachInfos;
	attachInfos.resize(attachNum);
	for (i = 0; i < attachNum; ++i, ++index) {

		attachInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		attachInfos[i].sampler = VK_NULL_HANDLE;
		attachInfos[i].imageView = attachImageViews[i];

		descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[index].pNext = nullptr;
		descriptorWrites[index].dstSet = descriptorSet;
		descriptorWrites[index].dstBinding = bindID[index];
		descriptorWrites[index].dstArrayElement = 0;
		descriptorWrites[index].descriptorCount = 1;
		descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		descriptorWrites[index].pImageInfo = &attachInfos[i];
		descriptorWrites[index].pBufferInfo = nullptr;
		descriptorWrites[index].pTexelBufferView = nullptr;
	}

	vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

VkShaderModule QeVulkan::createShaderModel(void* data, VkDeviceSize size) {
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = size;
	createInfo.pCode = reinterpret_cast<const uint32_t*>(data);
	
	VkShaderModule shader;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shader) != VK_SUCCESS) LOG("failed to create shader module!");

	return shader;
}

VkSampler QeVulkan::createTextureSampler() {
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;// VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;// VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;// VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.maxAnisotropy = 1;// 16;
	samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;// VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

	samplerInfo.mipLodBias = 0;
	samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.minLod = 0;
	samplerInfo.maxLod = 1;


	VkSampler sampler;
	if (vkCreateSampler(VK->device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) LOG("failed to create texture sampler!");
	
	return sampler;
}

void QeVulkan::createImageData(void* data, VkFormat format, VkDeviceSize imageSize,  int width, int height, VkImage& image, VkDeviceMemory& imageMemory, int layer, bool bCubemap) {
	
	QeVKBuffer staging;
	createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging.buffer, staging.memory);

	setMemory(staging.memory, data, imageSize);
	
	if(image== VK_NULL_HANDLE)
		createImage(width, height, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, imageMemory, bCubemap);

	if(bCubemap)
		transitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	else
		transitionImageLayout(image, format, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	copyBufferToImage(staging.buffer, image, static_cast<uint32_t>(width), static_cast<uint32_t>(height), layer);

	transitionImageLayout(image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void QeVulkan::createBufferData(void* data, VkDeviceSize bufferSize, VkBuffer& buffer, VkDeviceMemory& bufferMemory){

	QeVKBuffer staging;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging.buffer, staging.memory);
	
	setMemory(staging.memory, data, bufferSize);
	
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);

	copyBuffer(staging.buffer, buffer, bufferSize);
}

void QeVulkan::createUniformBuffer( VkDeviceSize bufferSize, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
	createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffer, bufferMemory);
}

/*
// Vulkan Cookbook
namespace VulkanCookbook {

	bool IsExtensionSupported(std::vector<VkExtensionProperties> const & available_extensions, char const * const extension) {
		for (auto & available_extension : available_extensions) {
			if (strstr(available_extension.extensionName, extension)) {
				return true;
			}
		}
		return false;
	}

	bool CheckAvailableInstanceExtensions(std::vector<VkExtensionProperties> & available_extensions) {
		uint32_t extensions_count = 0;
		VkResult result = VK_SUCCESS;

		result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);
		if ((result != VK_SUCCESS) ||
			(extensions_count == 0)) {
			LOG("Could not get the number of instance extensions.");
			return false;
		}

		available_extensions.resize(extensions_count);
		result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, available_extensions.data());
		if ((result != VK_SUCCESS) ||
			(extensions_count == 0)) {
			LOG("Could not enumerate instance extensions.");
			return false;
		}

		return true;
	}

	bool CreateVulkanInstance(std::vector<char const*> const & desired_extensions, VkInstance & instance) {
		std::vector<VkExtensionProperties> available_extensions;
		if (!CheckAvailableInstanceExtensions(available_extensions)) {
			return false;
		}

		for (auto & extension : desired_extensions) {
			if (!IsExtensionSupported(available_extensions, extension)) {
				LOG("Extension named '" + extension + "' is not supported by an Instance object.");
				return false;
			}
		}
		std::vector<std::string> vsApplicationVersion = ENCODE->split(AST->getXMLValue(2, AST->CONFIG, "applicationVersion"), ".");
		std::vector<std::string> vsEnginVersion = ENCODE->split(AST->getXMLValue(2, AST->CONFIG, "engineVersion"), ".");
		std::vector<std::string> vsVulkanAPIVersion = ENCODE->split(AST->getXMLValue(2, AST->CONFIG, "VulkanAPIVersion"), ".");

		VkApplicationInfo application_info = {
			VK_STRUCTURE_TYPE_APPLICATION_INFO,                 // VkStructureType           sType
			nullptr,                                            // const void              * pNext
			AST->getXMLValue(2, AST->CONFIG, "applicationName"),// const char              * pApplicationName
			VK_MAKE_VERSION(static_cast<uint32_t>(atoi(vsApplicationVersion[0].c_str())), static_cast<uint32_t>(atoi(vsApplicationVersion[1].c_str())), static_cast<uint32_t>(atoi(vsApplicationVersion[2].c_str()))),
																// uint32_t                  applicationVersio
			AST->getXMLValue(2, AST->CONFIG, "engineName"),     // const char              * pEngineName
			VK_MAKE_VERSION(static_cast<uint32_t>(atoi(vsEnginVersion[0].c_str())), static_cast<uint32_t>(atoi(vsEnginVersion[1].c_str())), static_cast<uint32_t>(atoi(vsEnginVersion[2].c_str()))),
																// uint32_t                  engineVersion
			VK_MAKE_VERSION(static_cast<uint32_t>(atoi(vsVulkanAPIVersion[0].c_str())), static_cast<uint32_t>(atoi(vsVulkanAPIVersion[1].c_str())), static_cast<uint32_t>(atoi(vsVulkanAPIVersion[2].c_str()))),
																// uint32_t                  apiVersion
		};

		VkInstanceCreateInfo instance_create_info = {
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,             // VkStructureType           sType
			nullptr,                                            // const void              * pNext
			0,                                                  // VkInstanceCreateFlags     flags
			&application_info,                                  // const VkApplicationInfo * pApplicationInfo
			0,                                                  // uint32_t                  enabledLayerCount
			nullptr,                                            // const char * const      * ppEnabledLayerNames
			static_cast<uint32_t>(desired_extensions.size()),   // uint32_t                  enabledExtensionCount
			desired_extensions.data()                           // const char * const      * ppEnabledExtensionNames
		};

		VkResult result = vkCreateInstance(&instance_create_info, nullptr, &instance);
		if ((result != VK_SUCCESS) ||
			(instance == VK_NULL_HANDLE)) {
			LOG("Could not create Vulkan instance.");
			return false;
		}

		return true;
	}

	bool EnumerateAvailablePhysicalDevices(VkInstance instance, std::vector<VkPhysicalDevice>& available_devices) {
		uint32_t devices_count = 0;
		VkResult result = VK_SUCCESS;

		result = vkEnumeratePhysicalDevices(instance, &devices_count, nullptr);
		if ((result != VK_SUCCESS) ||
			(devices_count == 0)) {
			LOG("Could not get the number of available physical devices.");
			return false;
		}

		available_devices.resize(devices_count);
		result = vkEnumeratePhysicalDevices(instance, &devices_count, available_devices.data());
		if ((result != VK_SUCCESS) ||
			(devices_count == 0)) {
			LOG("Could not enumerate physical devices.");
			return false;
		}

		return true;
	}

	bool CheckAvailableDeviceExtensions(VkPhysicalDevice physical_device, std::vector<VkExtensionProperties>& available_extensions)
	{
		uint32_t extensions_count = 0;
		VkResult result = VK_SUCCESS;

		result = vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extensions_count, nullptr);
		if ((result != VK_SUCCESS) ||
			(extensions_count == 0)) {
			LOG("Could not get the number of device extensions.");
			return false;
		}

		available_extensions.resize(extensions_count);
		result = vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extensions_count, available_extensions.data());
		if ((result != VK_SUCCESS) ||
			(extensions_count == 0)) {
			LOG("Could not enumerate device extensions.");
			return false;
		}
		return true;
	}

	void GetFeaturesAndPropertiesOfPhysicalDevice(VkPhysicalDevice physical_device, VkPhysicalDeviceFeatures & device_features, VkPhysicalDeviceProperties & device_properties) {
		vkGetPhysicalDeviceFeatures(physical_device, &device_features);
		vkGetPhysicalDeviceProperties(physical_device, &device_properties);
	}

	bool CheckAvailableQueueFamiliesAndTheirProperties(VkPhysicalDevice physical_device, std::vector<VkQueueFamilyProperties>& queue_families) {
		uint32_t queue_families_count = 0;

		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, nullptr);
		if (queue_families_count == 0) {
			LOG("Could not get the number of queue families.");
			return false;
		}

		queue_families.resize(queue_families_count);
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, queue_families.data());
		if (queue_families_count == 0) {
			LOG("Could not acquire properties of queue families.");
			return false;
		}

		return true;
	}

	bool SelectIndexOfQueueFamilyWithDesiredCapabilities(VkPhysicalDevice physical_device, VkQueueFlags desired_capabilities, uint32_t & queue_family_index) {
		std::vector<VkQueueFamilyProperties> queue_families;
		if (!CheckAvailableQueueFamiliesAndTheirProperties(physical_device, queue_families)) {
			return false;
		}

		for (uint32_t index = 0; index < static_cast<uint32_t>(queue_families.size()); ++index) {
			if ((queue_families[index].queueCount > 0) &&
				((queue_families[index].queueFlags & desired_capabilities) == desired_capabilities)) {
				queue_family_index = index;
				return true;
			}
		}
		return false;
	}

	bool CreateLogicalDevice(VkPhysicalDevice physical_device, std::vector<QueueInfo> queue_infos, std::vector<char const*> const & desired_extensions, VkPhysicalDeviceFeatures * desired_features, VkDevice & logical_device) {
		std::vector<VkExtensionProperties> available_extensions;
		if (!CheckAvailableDeviceExtensions(physical_device, available_extensions)) {
			return false;
		}

		for (auto & extension : desired_extensions) {
			if (!IsExtensionSupported(available_extensions, extension)) {
				LOG("Extension named '" + extension + "' is not supported by a physical device.");
				return false;
			}
		}

		std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
		for (auto & info : queue_infos) {
			queue_create_infos.push_back({
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,       // VkStructureType                  sType
				nullptr,                                          // const void                     * pNext
				0,                                                // VkDeviceQueueCreateFlags         flags
				info.FamilyIndex,                                 // uint32_t                         queueFamilyIndex
				static_cast<uint32_t>(info.Priorities.size()),    // uint32_t                         queueCount
				info.Priorities.data()                            // const float                    * pQueuePriorities
				});
		};

		VkDeviceCreateInfo device_create_info = {
			VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,               // VkStructureType                  sType
			nullptr,                                            // const void                     * pNext
			0,                                                  // VkDeviceCreateFlags              flags
			static_cast<uint32_t>(queue_create_infos.size()),   // uint32_t                         queueCreateInfoCount
			queue_create_infos.data(),                          // const VkDeviceQueueCreateInfo  * pQueueCreateInfos
			0,                                                  // uint32_t                         enabledLayerCount
			nullptr,                                            // const char * const             * ppEnabledLayerNames
			static_cast<uint32_t>(desired_extensions.size()),   // uint32_t                         enabledExtensionCount
			desired_extensions.data(),                          // const char * const             * ppEnabledExtensionNames
			desired_features                                    // const VkPhysicalDeviceFeatures * pEnabledFeatures
		};

		VkResult result = vkCreateDevice(physical_device, &device_create_info, nullptr, &logical_device);
		if ((result != VK_SUCCESS) ||
			(logical_device == VK_NULL_HANDLE)) {
			LOG("Could not create logical device.");
			return false;
		}

		return true;
	}

	void GetDeviceQueue(VkDevice logical_device, uint32_t queue_family_index, uint32_t queue_index, VkQueue & queue) {
		vkGetDeviceQueue(logical_device, queue_family_index, queue_index, &queue);
	}

	bool CreateLogicalDeviceWithGeometryShadersAndGraphicsAndComputeQueues(VkInstance instance, VkDevice & logical_device, VkQueue & graphics_queue, VkQueue & compute_queue) {
		std::vector<VkPhysicalDevice> physical_devices;
		EnumerateAvailablePhysicalDevices(instance, physical_devices);

		for (auto & physical_device : physical_devices) {
			VkPhysicalDeviceFeatures device_features;
			VkPhysicalDeviceProperties device_properties;
			GetFeaturesAndPropertiesOfPhysicalDevice(physical_device, device_features, device_properties);

			if (!device_features.geometryShader) {
				continue;
			}
			else {
				device_features = {};
				device_features.geometryShader = VK_TRUE;
			}

			uint32_t graphics_queue_family_index;
			if (!SelectIndexOfQueueFamilyWithDesiredCapabilities(physical_device, VK_QUEUE_GRAPHICS_BIT, graphics_queue_family_index)) {
				continue;
			}

			uint32_t compute_queue_family_index;
			if (!SelectIndexOfQueueFamilyWithDesiredCapabilities(physical_device, VK_QUEUE_COMPUTE_BIT, compute_queue_family_index)) {
				continue;
			}

			std::vector<QueueInfo> requested_queues = { { graphics_queue_family_index,{ 1.0f } } };
			if (graphics_queue_family_index != compute_queue_family_index) {
				requested_queues.push_back({ compute_queue_family_index,{ 1.0f } });
			}
			if (!CreateLogicalDevice(physical_device, requested_queues, {}, &device_features, logical_device)) {
				continue;
			}
			else {
				GetDeviceQueue(logical_device, graphics_queue_family_index, 0, graphics_queue);
				GetDeviceQueue(logical_device, compute_queue_family_index, 0, compute_queue);
				return true;
			}
		}
		return false;
	}

	void DestroyLogicalDevice(VkDevice & logical_device) {
		if (logical_device) {
			vkDestroyDevice(logical_device, nullptr);
			logical_device = VK_NULL_HANDLE;
		}
	}

	void DestroyVulkanInstance(VkInstance & instance) {
		if (instance) {
			vkDestroyInstance(instance, nullptr);
			instance = VK_NULL_HANDLE;
		}
	}

	bool CreateVulkanInstanceWithWsiExtensionsEnabled(std::vector<char const *> & desired_extensions, VkInstance & instance) {
		desired_extensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
		desired_extensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

		return CreateVulkanInstance(desired_extensions, instance);
	}

	bool CreatePresentationSurface(VkInstance instance, HINSTANCE HInstance, HWND HWnd, VkSurfaceKHR & presentation_surface) {
		VkResult result;

		VkWin32SurfaceCreateInfoKHR surface_create_info = {
			VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,  // VkStructureType                 sType
			nullptr,                                          // const void                    * pNext
			0,                                                // VkWin32SurfaceCreateFlagsKHR    flags
			HInstance,					   					  // HINSTANCE                       hinstance
			HWnd											  // HWND                            hwnd
		};

		result = vkCreateWin32SurfaceKHR(instance, &surface_create_info, nullptr, &presentation_surface);

		if ((VK_SUCCESS != result) ||
			(VK_NULL_HANDLE == presentation_surface)) {
			LOG("Could not create presentation surface.");
			return false;
		}
		return true;
	}

	bool SelectQueueFamilyThatSupportsPresentationToGivenSurface(VkPhysicalDevice physical_device, VkSurfaceKHR presentation_surface, uint32_t & queue_family_index) {
		std::vector<VkQueueFamilyProperties> queue_families;
		if (!CheckAvailableQueueFamiliesAndTheirProperties(physical_device, queue_families)) {
			return false;
		}

		for (uint32_t index = 0; index < static_cast<uint32_t>(queue_families.size()); ++index) {
			VkBool32 presentation_supported = VK_FALSE;
			VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, index, presentation_surface, &presentation_supported);
			if ((VK_SUCCESS == result) &&
				(VK_TRUE == presentation_supported)) {
				queue_family_index = index;
				return true;
			}
		}
		return false;
	}

	bool CreateLogicalDeviceWithWsiExtensionsEnabled(VkPhysicalDevice physical_device, std::vector<QueueInfo> queue_infos, std::vector<char const*>& desired_extensions, VkPhysicalDeviceFeatures * desired_features, VkDevice & logical_device) {
		desired_extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		return CreateLogicalDevice(physical_device, queue_infos, desired_extensions, desired_features, logical_device);
	}

	bool SelectDesiredPresentationMode(VkPhysicalDevice physical_device, VkSurfaceKHR presentation_surface, VkPresentModeKHR desired_present_mode, VkPresentModeKHR & present_mode) {
		// Enumerate supported present modes
		uint32_t present_modes_count = 0;
		VkResult result = VK_SUCCESS;

		result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, presentation_surface, &present_modes_count, nullptr);
		if ((VK_SUCCESS != result) ||
			(0 == present_modes_count)) {
			LOG("Could not get the number of supported present modes.");
			return false;
		}

		std::vector<VkPresentModeKHR> present_modes(present_modes_count);
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, presentation_surface, &present_modes_count, present_modes.data());
		if ((VK_SUCCESS != result) ||
			(0 == present_modes_count)) {
			LOG("Could not enumerate present modes.");
			return false;
		}

		// Select present mode
		for (auto & current_present_mode : present_modes) {
			if (current_present_mode == desired_present_mode) {
				present_mode = desired_present_mode;
				return true;
			}
		}

		LOG("Desired present mode is not supported. Selecting default FIFO mode.");
		for (auto & current_present_mode : present_modes) {
			if (current_present_mode == VK_PRESENT_MODE_FIFO_KHR) {
				present_mode = VK_PRESENT_MODE_FIFO_KHR;
				return true;
			}
		}

		LOG("VK_PRESENT_MODE_FIFO_KHR is not supported though it's mandatory for all drivers!");
		return false;
	}

	bool GetCapabilitiesOfPresentationSurface(VkPhysicalDevice physical_device, VkSurfaceKHR presentation_surface, VkSurfaceCapabilitiesKHR & surface_capabilities) {
		VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, presentation_surface, &surface_capabilities);

		if (VK_SUCCESS != result) {
			LOG("Could not get the capabilities of a presentation surface.");
			return false;
		}
		return true;
	}

	bool SelectNumberOfSwapchainImages(VkSurfaceCapabilitiesKHR const & surface_capabilities, uint32_t & number_of_images) {
		number_of_images = surface_capabilities.minImageCount + 1;
		if ((surface_capabilities.maxImageCount > 0) &&
			(number_of_images > surface_capabilities.maxImageCount)) {
			number_of_images = surface_capabilities.maxImageCount;
		}
		return true;
	}

	bool ChooseSizeOfSwapchainImages(VkSurfaceCapabilitiesKHR const & surface_capabilities, VkExtent2D & size_of_images) {
		if (0xFFFFFFFF == surface_capabilities.currentExtent.width) {
			size_of_images = { 640, 480 };

			if (size_of_images.width < surface_capabilities.minImageExtent.width) {
				size_of_images.width = surface_capabilities.minImageExtent.width;
			}
			else if (size_of_images.width > surface_capabilities.maxImageExtent.width) {
				size_of_images.width = surface_capabilities.maxImageExtent.width;
			}

			if (size_of_images.height < surface_capabilities.minImageExtent.height) {
				size_of_images.height = surface_capabilities.minImageExtent.height;
			}
			else if (size_of_images.height > surface_capabilities.maxImageExtent.height) {
				size_of_images.height = surface_capabilities.maxImageExtent.height;
			}
		}
		else {
			size_of_images = surface_capabilities.currentExtent;
		}
		return true;
	}

	bool SelectDesiredUsageScenariosOfSwapchainImages(VkSurfaceCapabilitiesKHR const & surface_capabilities, VkImageUsageFlags desired_usages, VkImageUsageFlags & image_usage) {
		image_usage = desired_usages & surface_capabilities.supportedUsageFlags;

		return desired_usages == image_usage;
	}

	bool SelectTransformationOfSwapchainImages(VkSurfaceCapabilitiesKHR const & surface_capabilities, VkSurfaceTransformFlagBitsKHR desired_transform, VkSurfaceTransformFlagBitsKHR & surface_transform) {
		if (surface_capabilities.supportedTransforms & desired_transform) {
			surface_transform = desired_transform;
		}
		else {
			surface_transform = surface_capabilities.currentTransform;
		}
		return true;
	}

	bool SelectFormatOfSwapchainImages(VkPhysicalDevice physical_device, VkSurfaceKHR presentation_surface, VkSurfaceFormatKHR desired_surface_format, VkFormat & image_format, VkColorSpaceKHR & image_color_space) {
		// Enumerate supported formats
		uint32_t formats_count = 0;
		VkResult result = VK_SUCCESS;

		result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, presentation_surface, &formats_count, nullptr);
		if ((VK_SUCCESS != result) ||
			(0 == formats_count)) {
			LOG("Could not get the number of supported surface formats.");
			return false;
		}

		std::vector<VkSurfaceFormatKHR> surface_formats(formats_count);
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, presentation_surface, &formats_count, surface_formats.data());
		if ((VK_SUCCESS != result) ||
			(0 == formats_count)) {
			LOG("Could not enumerate supported surface formats.");
			return false;
		}

		// Select surface format
		if ((1 == surface_formats.size()) &&
			(VK_FORMAT_UNDEFINED == surface_formats[0].format)) {
			image_format = desired_surface_format.format;
			image_color_space = desired_surface_format.colorSpace;
			return true;
		}

		for (auto & surface_format : surface_formats) {
			if ((desired_surface_format.format == surface_format.format) &&
				(desired_surface_format.colorSpace == surface_format.colorSpace)) {
				image_format = desired_surface_format.format;
				image_color_space = desired_surface_format.colorSpace;
				return true;
			}
		}

		for (auto & surface_format : surface_formats) {
			if ((desired_surface_format.format == surface_format.format)) {
				image_format = desired_surface_format.format;
				image_color_space = surface_format.colorSpace;
				LOG("Desired combination of format and colorspace is not supported. Selecting other colorspace.");
				return true;
			}
		}

		image_format = surface_formats[0].format;
		image_color_space = surface_formats[0].colorSpace;
		LOG("Desired format is not supported. Selecting available format - colorspace combination.");
		return true;
	}

	bool CreateSwapchain(VkDevice logical_device, VkSurfaceKHR presentation_surface, uint32_t image_count, VkSurfaceFormatKHR surface_format, VkExtent2D image_size, VkImageUsageFlags image_usage, VkSurfaceTransformFlagBitsKHR surface_transform, VkPresentModeKHR present_mode, VkSwapchainKHR & old_swapchain, VkSwapchainKHR & swapchain) {
		VkSwapchainCreateInfoKHR swapchain_create_info = {
			VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,  // VkStructureType                  sType
			nullptr,                                      // const void                     * pNext
			0,                                            // VkSwapchainCreateFlagsKHR        flags
			presentation_surface,                         // VkSurfaceKHR                     surface
			image_count,                                  // uint32_t                         minImageCount
			surface_format.format,                        // VkFormat                         imageFormat
			surface_format.colorSpace,                    // VkColorSpaceKHR                  imageColorSpace
			image_size,                                   // VkExtent2D                       imageExtent
			1,                                            // uint32_t                         imageArrayLayers
			image_usage,                                  // VkImageUsageFlags                imageUsage
			VK_SHARING_MODE_EXCLUSIVE,                    // VkSharingMode                    imageSharingMode
			0,                                            // uint32_t                         queueFamilyIndexCount
			nullptr,                                      // const uint32_t                 * pQueueFamilyIndices
			surface_transform,                            // VkSurfaceTransformFlagBitsKHR    preTransform
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,            // VkCompositeAlphaFlagBitsKHR      compositeAlpha
			present_mode,                                 // VkPresentModeKHR                 presentMode
			VK_TRUE,                                      // VkBool32                         clipped
			old_swapchain                                 // VkSwapchainKHR                   oldSwapchain
		};

		VkResult result = vkCreateSwapchainKHR(logical_device, &swapchain_create_info, nullptr, &swapchain);
		if ((VK_SUCCESS != result) ||
			(VK_NULL_HANDLE == swapchain)) {
			LOG("Could not create a swapchain.");
			return false;
		}

		if (VK_NULL_HANDLE != old_swapchain) {
			vkDestroySwapchainKHR(logical_device, old_swapchain, nullptr);
			old_swapchain = VK_NULL_HANDLE;
		}

		return true;
	}

	bool GetHandlesOfSwapchainImages(VkDevice logical_device, VkSwapchainKHR swapchain, std::vector<VkImage>& swapchain_images) {
		uint32_t images_count = 0;
		VkResult result = VK_SUCCESS;

		result = vkGetSwapchainImagesKHR(logical_device, swapchain, &images_count, nullptr);
		if ((VK_SUCCESS != result) ||
			(0 == images_count)) {
			LOG("Could not get the number of swapchain images.");
			return false;
		}

		swapchain_images.resize(images_count);
		result = vkGetSwapchainImagesKHR(logical_device, swapchain, &images_count, swapchain_images.data());
		if ((VK_SUCCESS != result) ||
			(0 == images_count)) {
			LOG("Could not enumerate swapchain images.");
			return false;
		}

		return true;
	}

	bool CreateSwapchainWithR8G8B8A8FormatAndMailboxPresentMode(VkPhysicalDevice physical_device, VkSurfaceKHR presentation_surface, VkDevice logical_device, VkImageUsageFlags swapchain_image_usage, VkExtent2D & image_size, VkFormat & image_format, VkSwapchainKHR & old_swapchain, VkSwapchainKHR & swapchain, std::vector<VkImage>& swapchain_images) {
		VkPresentModeKHR desired_present_mode;
		if (!SelectDesiredPresentationMode(physical_device, presentation_surface, VK_PRESENT_MODE_MAILBOX_KHR, desired_present_mode)) {
			return false;
		}

		VkSurfaceCapabilitiesKHR surface_capabilities;
		if (!GetCapabilitiesOfPresentationSurface(physical_device, presentation_surface, surface_capabilities)) {
			return false;
		}

		uint32_t number_of_images;
		if (!SelectNumberOfSwapchainImages(surface_capabilities, number_of_images)) {
			return false;
		}

		if (!ChooseSizeOfSwapchainImages(surface_capabilities, image_size)) {
			return false;
		}
		if ((0 == image_size.width) ||
			(0 == image_size.height)) {
			return true;
		}

		VkImageUsageFlags image_usage;
		if (!SelectDesiredUsageScenariosOfSwapchainImages(surface_capabilities, swapchain_image_usage, image_usage)) {
			return false;
		}

		VkSurfaceTransformFlagBitsKHR surface_transform;
		SelectTransformationOfSwapchainImages(surface_capabilities, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR, surface_transform);

		VkColorSpaceKHR image_color_space;
		if (!SelectFormatOfSwapchainImages(physical_device, presentation_surface, { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }, image_format, image_color_space)) {
			return false;
		}

		if (!CreateSwapchain(logical_device, presentation_surface, number_of_images, { image_format, image_color_space }, image_size, image_usage, surface_transform, desired_present_mode, old_swapchain, swapchain)) {
			return false;
		}

		if (!GetHandlesOfSwapchainImages(logical_device, swapchain, swapchain_images)) {
			return false;
		}
		return true;
	}

	bool AcquireSwapchainImage(VkDevice logical_device, VkSwapchainKHR swapchain, VkSemaphore semaphore, VkFence fence, uint32_t & image_index) {
		VkResult result;

		result = vkAcquireNextImageKHR(logical_device, swapchain, 2000000000, semaphore, fence, &image_index);
		switch (result) {
		case VK_SUCCESS:
		case VK_SUBOPTIMAL_KHR:
			return true;
		default:
			return false;
		}
	}

	bool PresentImage(VkQueue queue, std::vector<VkSemaphore> rendering_semaphores, std::vector<PresentInfo> images_to_present) {
		VkResult result;
		std::vector<VkSwapchainKHR> swapchains;
		std::vector<uint32_t> image_indices;

		for (auto & image_to_present : images_to_present) {
			swapchains.emplace_back(image_to_present.Swapchain);
			image_indices.emplace_back(image_to_present.ImageIndex);
		}

		VkPresentInfoKHR present_info = {
			VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,                   // VkStructureType          sType
			nullptr,                                              // const void*              pNext
			static_cast<uint32_t>(rendering_semaphores.size()),   // uint32_t                 waitSemaphoreCount
			rendering_semaphores.data(),                          // const VkSemaphore      * pWaitSemaphores
			static_cast<uint32_t>(swapchains.size()),             // uint32_t                 swapchainCount
			swapchains.data(),                                    // const VkSwapchainKHR   * pSwapchains
			image_indices.data(),                                 // const uint32_t         * pImageIndices
			nullptr                                               // VkResult*                pResults
		};

		result = vkQueuePresentKHR(queue, &present_info);
		switch (result) {
		case VK_SUCCESS:
			return true;
		default:
			return false;
		}
	}

	void DestroySwapchain(VkDevice logical_device, VkSwapchainKHR & swapchain) {
		if (swapchain) {
			vkDestroySwapchainKHR(logical_device, swapchain, nullptr);
			swapchain = VK_NULL_HANDLE;
		}
	}

	void DestroyPresentationSurface(VkInstance instance, VkSurfaceKHR & presentation_surface) {
		if (presentation_surface) {
			vkDestroySurfaceKHR(instance, presentation_surface, nullptr);
			presentation_surface = VK_NULL_HANDLE;
		}
	}

	bool CreateCommandPool(VkDevice logical_device, VkCommandPoolCreateFlags parameters, uint32_t queue_family, VkCommandPool & command_pool) {
		VkCommandPoolCreateInfo command_pool_create_info = {
			VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,   // VkStructureType              sType
			nullptr,                                      // const void                 * pNext
			parameters,                                   // VkCommandPoolCreateFlags     flags
			queue_family                                  // uint32_t                     queueFamilyIndex
		};

		VkResult result = vkCreateCommandPool(logical_device, &command_pool_create_info, nullptr, &command_pool);
		if (VK_SUCCESS != result) {
			LOG( "Could not create command pool." );
			return false;
		}
		return true;
	}

	bool AllocateCommandBuffers(VkDevice logical_device, VkCommandPool command_pool, VkCommandBufferLevel level, uint32_t count, std::vector<VkCommandBuffer> & command_buffers) {
		VkCommandBufferAllocateInfo command_buffer_allocate_info = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,   // VkStructureType          sType
			nullptr,                                          // const void             * pNext
			command_pool,                                     // VkCommandPool            commandPool
			level,                                            // VkCommandBufferLevel     level
			count                                             // uint32_t                 commandBufferCount
		};

		command_buffers.resize(count);

		VkResult result = vkAllocateCommandBuffers(logical_device, &command_buffer_allocate_info, command_buffers.data());
		if (VK_SUCCESS != result) {
			LOG( "Could not allocate command buffers." );
			return false;
		}
		return true;
	}

	bool BeginCommandBufferRecordingOperation(VkCommandBuffer command_buffer, VkCommandBufferUsageFlags usage, VkCommandBufferInheritanceInfo * secondary_command_buffer_info) {
		VkCommandBufferBeginInfo command_buffer_begin_info = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,    // VkStructureType                        sType
			nullptr,                                        // const void                           * pNext
			usage,                                          // VkCommandBufferUsageFlags              flags
			secondary_command_buffer_info                   // const VkCommandBufferInheritanceInfo * pInheritanceInfo
		};

		VkResult result = vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);
		if (VK_SUCCESS != result) {
			LOG( "Could not begin command buffer recording operation." );
			return false;
		}
		return true;
	}

	bool EndCommandBufferRecordingOperation(VkCommandBuffer command_buffer) {
		VkResult result = vkEndCommandBuffer(command_buffer);
		if (VK_SUCCESS != result) {
			LOG( "Error occurred during command buffer recording." );
			return false;
		}
		return true;
	}

	bool ResetCommandBuffer(VkCommandBuffer command_buffer, bool release_resources) {
		VkResult result = vkResetCommandBuffer(command_buffer, release_resources ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : 0);
		if (VK_SUCCESS != result) {
			LOG( "Error occurred during command buffer reset." );
			return false;
		}
		return true;
	}

	bool ResetCommandPool(VkDevice logical_device, VkCommandPool command_pool, bool release_resources) {
		VkResult result = vkResetCommandPool(logical_device, command_pool, release_resources ? VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT : 0);
		if (VK_SUCCESS != result) {
			LOG( "Error occurred during command pool reset." );
			return false;
		}
		return true;
	}

	bool CreateSemaphore(VkDevice logical_device, VkSemaphore & semaphore) {
		VkSemaphoreCreateInfo semaphore_create_info = {
			VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,    // VkStructureType            sType
			nullptr,                                    // const void               * pNext
			0                                           // VkSemaphoreCreateFlags     flags
		};

		VkResult result = vkCreateSemaphore(logical_device, &semaphore_create_info, nullptr, &semaphore);
		if (VK_SUCCESS != result) {
			LOG( "Could not create a semaphore." );
			return false;
		}
		return true;
	}

	bool CreateFence(VkDevice logical_device, bool signaled, VkFence & fence) {
		VkFenceCreateInfo fence_create_info = {
			VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,          // VkStructureType        sType
			nullptr,                                      // const void           * pNext
			signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0u, // VkFenceCreateFlags     flags
		};

		VkResult result = vkCreateFence(logical_device, &fence_create_info, nullptr, &fence);
		if (VK_SUCCESS != result) {
			LOG( "Could not create a fence." );
			return false;
		}
		return true;
	}

	bool WaitForFences(VkDevice logical_device, std::vector<VkFence> const & fences, VkBool32 wait_for_all, uint64_t timeout) {
		if (fences.size() > 0) {
			VkResult result = vkWaitForFences(logical_device, static_cast<uint32_t>(fences.size()), fences.data(), wait_for_all, timeout);
			if (VK_SUCCESS != result) {
				LOG( "Waiting on fence failed." );
				return false;
			}
			return true;
		}
		return false;
	}

	bool ResetFences(VkDevice logical_device, std::vector<VkFence> const & fences) {
		if (fences.size() > 0) {
			VkResult result = vkResetFences(logical_device, static_cast<uint32_t>(fences.size()), fences.data());
			if (VK_SUCCESS != result) {
				LOG( "Error occurred when tried to reset fences." );
				return false;
			}
			return VK_SUCCESS == result;
		}
		return false;
	}

	bool SubmitCommandBuffersToQueue(VkQueue queue, std::vector<WaitSemaphoreInfo> wait_semaphore_infos, std::vector<VkCommandBuffer> command_buffers, std::vector<VkSemaphore> signal_semaphores, VkFence fence) {
		std::vector<VkSemaphore>          wait_semaphore_handles;
		std::vector<VkPipelineStageFlags> wait_semaphore_stages;

		for (auto & wait_semaphore_info : wait_semaphore_infos) {
			wait_semaphore_handles.emplace_back(wait_semaphore_info.Semaphore);
			wait_semaphore_stages.emplace_back(wait_semaphore_info.WaitingStage);
		}

		VkSubmitInfo submit_info = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,                        // VkStructureType                sType
			nullptr,                                              // const void                   * pNext
			static_cast<uint32_t>(wait_semaphore_infos.size()),   // uint32_t                       waitSemaphoreCount
			wait_semaphore_handles.data(),                        // const VkSemaphore            * pWaitSemaphores
			wait_semaphore_stages.data(),                         // const VkPipelineStageFlags   * pWaitDstStageMask
			static_cast<uint32_t>(command_buffers.size()),        // uint32_t                       commandBufferCount
			command_buffers.data(),                               // const VkCommandBuffer        * pCommandBuffers
			static_cast<uint32_t>(signal_semaphores.size()),      // uint32_t                       signalSemaphoreCount
			signal_semaphores.data()                              // const VkSemaphore            * pSignalSemaphores
		};

		VkResult result = vkQueueSubmit(queue, 1, &submit_info, fence);
		if (VK_SUCCESS != result) {
			LOG( "Error occurred during command buffer submission." );
			return false;
		}
		return true;
	}

	bool SynchronizeTwoCommandBuffers(VkQueue first_queue, std::vector<WaitSemaphoreInfo> first_wait_semaphore_infos, std::vector<VkCommandBuffer> first_command_buffers, std::vector<WaitSemaphoreInfo> synchronizing_semaphores, VkQueue second_queue, std::vector<VkCommandBuffer> second_command_buffers, std::vector<VkSemaphore> second_signal_semaphores, VkFence second_fence) {
		std::vector<VkSemaphore> first_signal_semaphores;
		for (auto & semaphore_info : synchronizing_semaphores) {
			first_signal_semaphores.emplace_back(semaphore_info.Semaphore);
		}
		if (!SubmitCommandBuffersToQueue(first_queue, first_wait_semaphore_infos, first_command_buffers, first_signal_semaphores, VK_NULL_HANDLE)) {
			return false;
		}

		if (!SubmitCommandBuffersToQueue(second_queue, synchronizing_semaphores, second_command_buffers, second_signal_semaphores, second_fence)) {
			return false;
		}
		return true;
	}

	bool CheckIfProcessingOfSubmittedCommandBufferHasFinished(VkDevice logical_device, VkQueue queue, std::vector<WaitSemaphoreInfo> wait_semaphore_infos, std::vector<VkCommandBuffer> command_buffers, std::vector<VkSemaphore> signal_semaphores, VkFence fence, uint64_t timeout, VkResult & wait_status) {
		if (!SubmitCommandBuffersToQueue(queue, wait_semaphore_infos, command_buffers, signal_semaphores, fence)) {
			return false;
		}

		return WaitForFences(logical_device, { fence }, VK_FALSE, timeout);
	}

	bool WaitUntilAllCommandsSubmittedToQueueAreFinished(VkQueue queue) {
		VkResult result = vkQueueWaitIdle(queue);
		if (VK_SUCCESS != result) {
			LOG( "Waiting for all operations submitted to queue failed." );
			return false;
		}
		return true;
	}

	bool WaitForAllSubmittedCommandsToBeFinished(VkDevice logical_device) {
		VkResult result = vkDeviceWaitIdle(logical_device);
		if (VK_SUCCESS != result) {
			LOG( "Waiting on a device failed." );
			return false;
		}
		return true;
	}

	void DestroyFence(VkDevice logical_device, VkFence & fence) {
		if (VK_NULL_HANDLE != fence) {
			vkDestroyFence(logical_device, fence, nullptr);
			fence = VK_NULL_HANDLE;
		}
	}

	void DestroySemaphore(VkDevice logical_device, VkSemaphore & semaphore) {
		if (VK_NULL_HANDLE != semaphore) {
			vkDestroySemaphore(logical_device, semaphore, nullptr);
			semaphore = VK_NULL_HANDLE;
		}
	}

	void FreeCommandBuffers(VkDevice logical_device, VkCommandPool command_pool, std::vector<VkCommandBuffer> & command_buffers) {
		if (command_buffers.size() > 0) {
			vkFreeCommandBuffers(logical_device, command_pool, static_cast<uint32_t>(command_buffers.size()), command_buffers.data());
			command_buffers.clear();
		}
	}

	void DestroyCommandPool(VkDevice logical_device, VkCommandPool & command_pool) {
		if (VK_NULL_HANDLE != command_pool) {
			vkDestroyCommandPool(logical_device, command_pool, nullptr);
			command_pool = VK_NULL_HANDLE;
		}
	}

	bool CreateBuffer(VkDevice logical_device, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer & buffer) {
		VkBufferCreateInfo buffer_create_info = {
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,   // VkStructureType        sType
			nullptr,                                // const void           * pNext
			0,                                      // VkBufferCreateFlags    flags
			size,                                   // VkDeviceSize           size
			usage,                                  // VkBufferUsageFlags     usage
			VK_SHARING_MODE_EXCLUSIVE,              // VkSharingMode          sharingMode
			0,                                      // uint32_t               queueFamilyIndexCount
			nullptr                                 // const uint32_t       * pQueueFamilyIndices
		};

		VkResult result = vkCreateBuffer(logical_device, &buffer_create_info, nullptr, &buffer);
		if (VK_SUCCESS != result) {
			LOG("Could not create a buffer.");
			return false;
		}
		return true;
	}

	bool AllocateAndBindMemoryObjectToBuffer(VkPhysicalDevice physical_device, VkDevice logical_device, VkBuffer buffer, VkMemoryPropertyFlagBits memory_properties, VkDeviceMemory & memory_object) {
		VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
		vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_memory_properties);

		VkMemoryRequirements memory_requirements;
		vkGetBufferMemoryRequirements(logical_device, buffer, &memory_requirements);

		memory_object = VK_NULL_HANDLE;
		for (uint32_t type = 0; type < physical_device_memory_properties.memoryTypeCount; ++type) {
			if ((memory_requirements.memoryTypeBits & (1 << type)) &&
				((physical_device_memory_properties.memoryTypes[type].propertyFlags & memory_properties) == memory_properties)) {

				VkMemoryAllocateInfo buffer_memory_allocate_info = {
					VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,   // VkStructureType    sType
					nullptr,                                  // const void       * pNext
					memory_requirements.size,                 // VkDeviceSize       allocationSize
					type                                      // uint32_t           memoryTypeIndex
				};

				VkResult result = vkAllocateMemory(logical_device, &buffer_memory_allocate_info, nullptr, &memory_object);
				if (VK_SUCCESS == result) {
					break;
				}
			}
		}

		if (VK_NULL_HANDLE == memory_object) {
			LOG("Could not allocate memory for a buffer.");
			return false;
		}

		VkResult result = vkBindBufferMemory(logical_device, buffer, memory_object, 0);
		if (VK_SUCCESS != result) {
			LOG("Could not bind memory object to a buffer.");
			return false;
		}
		return true;
	}

	void SetBufferMemoryBarrier(VkCommandBuffer command_buffer, VkPipelineStageFlags generating_stages, VkPipelineStageFlags consuming_stages, std::vector<BufferTransition> buffer_transitions) {
		std::vector<VkBufferMemoryBarrier> buffer_memory_barriers;

		for (auto & buffer_transition : buffer_transitions) {
			buffer_memory_barriers.push_back({
				VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,    // VkStructureType    sType
				nullptr,                                    // const void       * pNext
				buffer_transition.CurrentAccess,            // VkAccessFlags      srcAccessMask
				buffer_transition.NewAccess,                // VkAccessFlags      dstAccessMask
				buffer_transition.CurrentQueueFamily,       // uint32_t           srcQueueFamilyIndex
				buffer_transition.NewQueueFamily,           // uint32_t           dstQueueFamilyIndex
				buffer_transition.Buffer,                   // VkBuffer           buffer
				0,                                          // VkDeviceSize       offset
				VK_WHOLE_SIZE                               // VkDeviceSize       size
				});
		}

		if (buffer_memory_barriers.size() > 0) {
			vkCmdPipelineBarrier(command_buffer, generating_stages, consuming_stages, 0, 0, nullptr, static_cast<uint32_t>(buffer_memory_barriers.size()), buffer_memory_barriers.data(), 0, nullptr);
		}
	}

	bool CreateBufferView(VkDevice logical_device, VkBuffer buffer, VkFormat format, VkDeviceSize memory_offset, VkDeviceSize memory_range, VkBufferView & buffer_view) {
		VkBufferViewCreateInfo buffer_view_create_info = {
			VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,    // VkStructureType            sType
			nullptr,                                      // const void               * pNext
			0,                                            // VkBufferViewCreateFlags    flags
			buffer,                                       // VkBuffer                   buffer
			format,                                       // VkFormat                   format
			memory_offset,                                // VkDeviceSize               offset
			memory_range                                  // VkDeviceSize               range
		};

		VkResult result = vkCreateBufferView(logical_device, &buffer_view_create_info, nullptr, &buffer_view);
		if (VK_SUCCESS != result) {
			LOG("Could not creat buffer view.");
			return false;
		}
		return true;
	}

	bool CreateImage(VkDevice logical_device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t num_mipmaps, uint32_t num_layers, VkSampleCountFlagBits samples, VkImageUsageFlags usage_scenarios, bool cubemap, VkImage & image) {
		VkImageCreateInfo image_create_info = {
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,                // VkStructureType          sType
			nullptr,                                            // const void             * pNext
			cubemap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0u, // VkImageCreateFlags       flags
			type,                                               // VkImageType              imageType
			format,                                             // VkFormat                 format
			size,                                               // VkExtent3D               extent
			num_mipmaps,                                        // uint32_t                 mipLevels
			cubemap ? 6 * num_layers : num_layers,              // uint32_t                 arrayLayers
			samples,                                            // VkSampleCountFlagBits    samples
			VK_IMAGE_TILING_OPTIMAL,                            // VkImageTiling            tiling
			usage_scenarios,                                    // VkImageUsageFlags        usage
			VK_SHARING_MODE_EXCLUSIVE,                          // VkSharingMode            sharingMode
			0,                                                  // uint32_t                 queueFamilyIndexCount
			nullptr,                                            // const uint32_t         * pQueueFamilyIndices
			VK_IMAGE_LAYOUT_UNDEFINED                           // VkImageLayout            initialLayout
		};

		VkResult result = vkCreateImage(logical_device, &image_create_info, nullptr, &image);
		if (VK_SUCCESS != result) {
			LOG("Could not create an image.");
			return false;
		}
		return true;
	}

	bool AllocateAndBindMemoryObjectToImage(VkPhysicalDevice physical_device, VkDevice logical_device, VkImage image, VkMemoryPropertyFlagBits memory_properties, VkDeviceMemory & memory_object) {
		VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
		vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_memory_properties);

		VkMemoryRequirements memory_requirements;
		vkGetImageMemoryRequirements(logical_device, image, &memory_requirements);

		memory_object = VK_NULL_HANDLE;
		for (uint32_t type = 0; type < physical_device_memory_properties.memoryTypeCount; ++type) {
			if ((memory_requirements.memoryTypeBits & (1 << type)) &&
				((physical_device_memory_properties.memoryTypes[type].propertyFlags & memory_properties) == memory_properties)) {

				VkMemoryAllocateInfo image_memory_allocate_info = {
					VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,   // VkStructureType    sType
					nullptr,                                  // const void       * pNext
					memory_requirements.size,                 // VkDeviceSize       allocationSize
					type                                      // uint32_t           memoryTypeIndex
				};

				VkResult result = vkAllocateMemory(logical_device, &image_memory_allocate_info, nullptr, &memory_object);
				if (VK_SUCCESS == result) {
					break;
				}
			}
		}

		if (VK_NULL_HANDLE == memory_object) {
			LOG("Could not allocate memory for an image.");
			return false;
		}

		VkResult result = vkBindImageMemory(logical_device, image, memory_object, 0);
		if (VK_SUCCESS != result) {
			LOG("Could not bind memory object to an image.");
			return false;
		}
		return true;
	}

	void SetImageMemoryBarrier(VkCommandBuffer command_buffer, VkPipelineStageFlags generating_stages, VkPipelineStageFlags consuming_stages, std::vector<ImageTransition> image_transitions) {
		std::vector<VkImageMemoryBarrier> image_memory_barriers;

		for (auto & image_transition : image_transitions) {
			image_memory_barriers.push_back({
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,   // VkStructureType            sType
				nullptr,                                  // const void               * pNext
				image_transition.CurrentAccess,           // VkAccessFlags              srcAccessMask
				image_transition.NewAccess,               // VkAccessFlags              dstAccessMask
				image_transition.CurrentLayout,           // VkImageLayout              oldLayout
				image_transition.NewLayout,               // VkImageLayout              newLayout
				image_transition.CurrentQueueFamily,      // uint32_t                   srcQueueFamilyIndex
				image_transition.NewQueueFamily,          // uint32_t                   dstQueueFamilyIndex
				image_transition.Image,                   // VkImage                    image
				{                                         // VkImageSubresourceRange    subresourceRange
					image_transition.Aspect,                  // VkImageAspectFlags         aspectMask
					0,                                        // uint32_t                   baseMipLevel
					VK_REMAINING_MIP_LEVELS,                  // uint32_t                   levelCount
					0,                                        // uint32_t                   baseArrayLayer
					VK_REMAINING_ARRAY_LAYERS                 // uint32_t                   layerCount
				}
				});
		}
		if (image_memory_barriers.size() > 0) {
			vkCmdPipelineBarrier(command_buffer, generating_stages, consuming_stages, 0, 0, nullptr, 0, nullptr, static_cast<uint32_t>(image_memory_barriers.size()), image_memory_barriers.data());
		}
	}

	bool CreateImageView(VkDevice logical_device, VkImage image, VkImageViewType view_type, VkFormat format, VkImageAspectFlags aspect, VkImageView & image_view) {
		VkImageViewCreateInfo image_view_create_info = {
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,   // VkStructureType            sType
			nullptr,                                    // const void               * pNext
			0,                                          // VkImageViewCreateFlags     flags
			image,                                      // VkImage                    image
			view_type,                                  // VkImageViewType            viewType
			format,                                     // VkFormat                   format
			{                                           // VkComponentMapping         components
				VK_COMPONENT_SWIZZLE_IDENTITY,          // VkComponentSwizzle         r
				VK_COMPONENT_SWIZZLE_IDENTITY,          // VkComponentSwizzle         g
				VK_COMPONENT_SWIZZLE_IDENTITY,          // VkComponentSwizzle         b
				VK_COMPONENT_SWIZZLE_IDENTITY           // VkComponentSwizzle         a
			},
			{                                           // VkImageSubresourceRange    subresourceRange
				aspect,                                 // VkImageAspectFlags         aspectMask
				0,                                      // uint32_t                   baseMipLevel
				VK_REMAINING_MIP_LEVELS,                // uint32_t                   levelCount
				0,                                      // uint32_t                   baseArrayLayer
				VK_REMAINING_ARRAY_LAYERS               // uint32_t                   layerCount
			}
		};

		VkResult result = vkCreateImageView(logical_device, &image_view_create_info, nullptr, &image_view);
		if (VK_SUCCESS != result) {
			LOG("Could not create an image view.");
			return false;
		}
		return true;
	}

	bool Create2DImageAndView(VkPhysicalDevice physical_device, VkDevice logical_device, VkFormat format, VkExtent2D size, uint32_t num_mipmaps, uint32_t num_layers, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageAspectFlags aspect, VkImage & image, VkDeviceMemory & memory_object, VkImageView & image_view) {
		if (!CreateImage(logical_device, VK_IMAGE_TYPE_2D, format, { size.width, size.height, 1 }, num_mipmaps, num_layers, samples, usage, false, image)) {
			return false;
		}

		if (!AllocateAndBindMemoryObjectToImage(physical_device, logical_device, image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memory_object)) {
			return false;
		}

		if (!CreateImageView(logical_device, image, VK_IMAGE_VIEW_TYPE_2D, format, aspect, image_view)) {
			return false;
		}

		return true;
	}

	bool CreateLayered2DImageWithCubemapView(VkPhysicalDevice physical_device, VkDevice logical_device, uint32_t size, uint32_t num_mipmaps, VkImageUsageFlags usage, VkImageAspectFlags aspect, VkImage & image, VkDeviceMemory & memory_object, VkImageView & image_view) {
		if (!CreateImage(logical_device, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, { size, size, 1 }, num_mipmaps, 6, VK_SAMPLE_COUNT_1_BIT, usage, true, image)) {
			return false;
		}

		if (!AllocateAndBindMemoryObjectToImage(physical_device, logical_device, image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memory_object)) {
			return false;
		}

		if (!CreateImageView(logical_device, image, VK_IMAGE_VIEW_TYPE_CUBE, VK_FORMAT_R8G8B8A8_UNORM, aspect, image_view)) {
			return false;
		}

		return true;
	}

	bool MapUpdateAndUnmapHostVisibleMemory(VkDevice logical_device, VkDeviceMemory memory_object, VkDeviceSize offset, VkDeviceSize data_size, void * data, bool unmap, void ** pointer) {
		VkResult result;
		void * local_pointer;
		result = vkMapMemory(logical_device, memory_object, offset, data_size, 0, &local_pointer);
		if (VK_SUCCESS != result) {
			LOG("Could not map memory object.");
			return false;
		}

		std::memcpy(local_pointer, data, static_cast<size_t>(data_size));

		std::vector<VkMappedMemoryRange> memory_ranges = {
			{
				VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,  // VkStructureType    sType
				nullptr,                                // const void       * pNext
				memory_object,                          // VkDeviceMemory     memory
				offset,                                 // VkDeviceSize       offset
				VK_WHOLE_SIZE                           // VkDeviceSize       size
			}
		};

		vkFlushMappedMemoryRanges(logical_device, static_cast<uint32_t>(memory_ranges.size()), memory_ranges.data());
		if (VK_SUCCESS != result) {
			LOG("Could not flush mapped memory.");
			return false;
		}

		if (unmap) {
			vkUnmapMemory(logical_device, memory_object);
		}
		else if (nullptr != pointer) {
			*pointer = local_pointer;
		}

		return true;
	}

	void CopyDataBetweenBuffers(VkCommandBuffer command_buffer, VkBuffer source_buffer, VkBuffer destination_buffer, std::vector<VkBufferCopy> regions) {
		if (regions.size() > 0) {
			vkCmdCopyBuffer(command_buffer, source_buffer, destination_buffer, static_cast<uint32_t>(regions.size()), regions.data());
		}
	}

	void CopyDataFromBufferToImage(VkCommandBuffer command_buffer, VkBuffer source_buffer, VkImage destination_image, VkImageLayout image_layout, std::vector<VkBufferImageCopy> regions) {
		if (regions.size() > 0) {
			vkCmdCopyBufferToImage(command_buffer, source_buffer, destination_image, image_layout, static_cast<uint32_t>(regions.size()), regions.data());
		}
	}

	void CopyDataFromImageToBuffer(VkCommandBuffer command_buffer, VkImage source_image, VkImageLayout image_layout, VkBuffer destination_buffer, std::vector<VkBufferImageCopy> regions) {
		if (regions.size() > 0) {
			vkCmdCopyImageToBuffer(command_buffer, source_image, image_layout, destination_buffer, static_cast<uint32_t>(regions.size()), regions.data());
		}
	}

	bool UseStagingBufferToUpdateBufferWithDeviceLocalMemoryBound(VkPhysicalDevice physical_device, VkDevice logical_device, VkDeviceSize data_size, void * data, VkBuffer destination_buffer, VkDeviceSize destination_offset, VkAccessFlags destination_buffer_current_access, VkAccessFlags destination_buffer_new_access, VkPipelineStageFlags destination_buffer_generating_stages, VkPipelineStageFlags destination_buffer_consuming_stages, VkQueue queue, VkCommandBuffer command_buffer, std::vector<VkSemaphore> signal_semaphores) {
		VkDestroyer(VkBuffer) staging_buffer;
		InitVkDestroyer(logical_device, staging_buffer);
		if (!CreateBuffer(logical_device, data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, *staging_buffer)) {
			return false;
		}

		VkDestroyer(VkDeviceMemory) memory_object;
		InitVkDestroyer(logical_device, memory_object);
		if (!AllocateAndBindMemoryObjectToBuffer(physical_device, logical_device, *staging_buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, *memory_object)) {
			return false;
		}

		if (!MapUpdateAndUnmapHostVisibleMemory(logical_device, *memory_object, 0, data_size, data, true, nullptr)) {
			return false;
		}

		if (!BeginCommandBufferRecordingOperation(command_buffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr)) {
			return false;
		}

		SetBufferMemoryBarrier(command_buffer, destination_buffer_generating_stages, VK_PIPELINE_STAGE_TRANSFER_BIT, { { destination_buffer, destination_buffer_current_access, VK_ACCESS_TRANSFER_WRITE_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED } });

		CopyDataBetweenBuffers(command_buffer, *staging_buffer, destination_buffer, { { 0, destination_offset, data_size } });

		SetBufferMemoryBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, destination_buffer_consuming_stages, { { destination_buffer, VK_ACCESS_TRANSFER_WRITE_BIT, destination_buffer_new_access, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED } });

		if (!EndCommandBufferRecordingOperation(command_buffer)) {
			return false;
		}

		VkDestroyer(VkFence) fence;
		InitVkDestroyer(logical_device, fence);
		if (!CreateFence(logical_device, false, *fence)) {
			return false;
		}

		if (!SubmitCommandBuffersToQueue(queue, {}, { command_buffer }, signal_semaphores, *fence)) {
			return false;
		}

		if (!WaitForFences(logical_device, { *fence }, VK_FALSE, 500000000)) {
			return false;
		}

		return true;
	}

	bool UseStagingBufferToUpdateImageWithDeviceLocalMemoryBound(VkPhysicalDevice physical_device, VkDevice logical_device, VkDeviceSize data_size, void * data, VkImage destination_image, VkImageSubresourceLayers destination_image_subresource, VkOffset3D destination_image_offset, VkExtent3D destination_image_size, VkImageLayout destination_image_current_layout, VkImageLayout destination_image_new_layout, VkAccessFlags destination_image_current_access, VkAccessFlags destination_image_new_access, VkImageAspectFlags destination_image_aspect, VkPipelineStageFlags destination_image_generating_stages, VkPipelineStageFlags destination_image_consuming_stages, VkQueue queue, VkCommandBuffer command_buffer, std::vector<VkSemaphore> signal_semaphores) {
		VkDestroyer(VkBuffer) staging_buffer;
		InitVkDestroyer(logical_device, staging_buffer);
		if (!CreateBuffer(logical_device, data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, *staging_buffer)) {
			return false;
		}

		VkDestroyer(VkDeviceMemory) memory_object;
		InitVkDestroyer(logical_device, memory_object);
		if (!AllocateAndBindMemoryObjectToBuffer(physical_device, logical_device, *staging_buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, *memory_object)) {
			return false;
		}

		if (!MapUpdateAndUnmapHostVisibleMemory(logical_device, *memory_object, 0, data_size, data, true, nullptr)) {
			return false;
		}

		if (!BeginCommandBufferRecordingOperation(command_buffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr)) {
			return false;
		}

		SetImageMemoryBarrier(command_buffer, destination_image_generating_stages, VK_PIPELINE_STAGE_TRANSFER_BIT,
			{
				{
					destination_image,                        // VkImage            Image
					destination_image_current_access,         // VkAccessFlags      CurrentAccess
					VK_ACCESS_TRANSFER_WRITE_BIT,             // VkAccessFlags      NewAccess
					destination_image_current_layout,         // VkImageLayout      CurrentLayout
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,     // VkImageLayout      NewLayout
					VK_QUEUE_FAMILY_IGNORED,                  // uint32_t           CurrentQueueFamily
					VK_QUEUE_FAMILY_IGNORED,                  // uint32_t           NewQueueFamily
					destination_image_aspect                  // VkImageAspectFlags Aspect
				} });

		CopyDataFromBufferToImage(command_buffer, *staging_buffer, destination_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			{
				{
					0,                                        // VkDeviceSize               bufferOffset
					0,                                        // uint32_t                   bufferRowLength
					0,                                        // uint32_t                   bufferImageHeight
					destination_image_subresource,            // VkImageSubresourceLayers   imageSubresource
					destination_image_offset,                 // VkOffset3D                 imageOffset
					destination_image_size,                   // VkExtent3D                 imageExtent
				} });

		SetImageMemoryBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, destination_image_consuming_stages,
			{
				{
					destination_image,                        // VkImage            Image
					VK_ACCESS_TRANSFER_WRITE_BIT,             // VkAccessFlags      CurrentAccess
					destination_image_new_access,             // VkAccessFlags      NewAccess
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,     // VkImageLayout      CurrentLayout
					destination_image_new_layout,             // VkImageLayout      NewLayout
					VK_QUEUE_FAMILY_IGNORED,                  // uint32_t           CurrentQueueFamily
					VK_QUEUE_FAMILY_IGNORED,                  // uint32_t           NewQueueFamily
					destination_image_aspect                  // VkImageAspectFlags Aspect
				} });

		if (!EndCommandBufferRecordingOperation(command_buffer)) {
			return false;
		}

		VkDestroyer(VkFence) fence;
		InitVkDestroyer(logical_device, fence);
		if (!CreateFence(logical_device, false, *fence)) {
			return false;
		}

		if (!SubmitCommandBuffersToQueue(queue, {}, { command_buffer }, signal_semaphores, *fence)) {
			return false;
		}

		if (!WaitForFences(logical_device, { *fence }, VK_FALSE, 500000000)) {
			return false;
		}

		return true;
	}

	void DestroyImageView(VkDevice logical_device, VkImageView & image_view) {
		if (VK_NULL_HANDLE != image_view) {
			vkDestroyImageView(logical_device, image_view, nullptr);
			image_view = VK_NULL_HANDLE;
		}
	}

	void DestroyImage(VkDevice logical_device, VkImage & image) {
		if (VK_NULL_HANDLE != image) {
			vkDestroyImage(logical_device, image, nullptr);
			image = VK_NULL_HANDLE;
		}
	}

	void DestroyBufferView(VkDevice logical_device, VkBufferView & buffer_view) {
		if (VK_NULL_HANDLE != buffer_view) {
			vkDestroyBufferView(logical_device, buffer_view, nullptr);
			buffer_view = VK_NULL_HANDLE;
		}
	}

	void FreeMemoryObject(VkDevice logical_device, VkDeviceMemory & memory_object) {
		if (VK_NULL_HANDLE != memory_object) {
			vkFreeMemory(logical_device, memory_object, nullptr);
			memory_object = VK_NULL_HANDLE;
		}
	}

	void DestroyBuffer(VkDevice logical_device, VkBuffer & buffer) {
		if (VK_NULL_HANDLE != buffer) {
			vkDestroyBuffer(logical_device, buffer, nullptr);
			buffer = VK_NULL_HANDLE;
		}
	}

	bool CreateSampler(VkDevice logical_device, VkFilter mag_filter, VkFilter min_filter, VkSamplerMipmapMode mipmap_mode, VkSamplerAddressMode u_address_mode, VkSamplerAddressMode v_address_mode, VkSamplerAddressMode w_address_mode, float lod_bias, bool anisotropy_enable, float max_anisotropy, bool compare_enable, VkCompareOp compare_operator, float min_lod, float max_lod, VkBorderColor border_color, bool unnormalized_coords, VkSampler & sampler) {
		VkSamplerCreateInfo sampler_create_info = {
			VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,    // VkStructureType          sType
			nullptr,                                  // const void             * pNext
			0,                                        // VkSamplerCreateFlags     flags
			mag_filter,                               // VkFilter                 magFilter
			min_filter,                               // VkFilter                 minFilter
			mipmap_mode,                              // VkSamplerMipmapMode      mipmapMode
			u_address_mode,                           // VkSamplerAddressMode     addressModeU
			v_address_mode,                           // VkSamplerAddressMode     addressModeV
			w_address_mode,                           // VkSamplerAddressMode     addressModeW
			lod_bias,                                 // float                    mipLodBias
			anisotropy_enable,                        // VkBool32                 anisotropyEnable
			max_anisotropy,                           // float                    maxAnisotropy
			compare_enable,                           // VkBool32                 compareEnable
			compare_operator,                         // VkCompareOp              compareOp
			min_lod,                                  // float                    minLod
			max_lod,                                  // float                    maxLod
			border_color,                             // VkBorderColor            borderColor
			unnormalized_coords                       // VkBool32                 unnormalizedCoordinates
		};

		VkResult result = vkCreateSampler(logical_device, &sampler_create_info, nullptr, &sampler);
		if (VK_SUCCESS != result) {
			LOG( "Could not create sampler." );
			return false;
		}
		return true;
	}

	bool CreateSampledImage(VkPhysicalDevice physical_device, VkDevice logical_device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t num_mipmaps, uint32_t num_layers, VkImageUsageFlags usage, bool cubemap, VkImageViewType view_type, VkImageAspectFlags aspect, bool linear_filtering, VkImage & sampled_image, VkDeviceMemory & memory_object, VkImageView & sampled_image_view) {
		VkFormatProperties format_properties;
		vkGetPhysicalDeviceFormatProperties(physical_device, format, &format_properties);
		if (!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)) {
			LOG( "Provided format is not supported for a sampled image." );
			return false;
		}
		if (linear_filtering &&
			!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
			LOG( "Provided format is not supported for a linear image filtering." );
			return false;
		}

		if (!CreateImage(logical_device, type, format, size, num_mipmaps, num_layers, VK_SAMPLE_COUNT_1_BIT, usage | VK_IMAGE_USAGE_SAMPLED_BIT, cubemap, sampled_image)) {
			return false;
		}

		if (!AllocateAndBindMemoryObjectToImage(physical_device, logical_device, sampled_image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memory_object)) {
			return false;
		}

		if (!CreateImageView(logical_device, sampled_image, view_type, format, aspect, sampled_image_view)) {
			return false;
		}
		return true;
	}

	bool CreateCombinedImageSampler(VkPhysicalDevice physical_device, VkDevice logical_device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t num_mipmaps, uint32_t num_layers, VkImageUsageFlags usage, bool cubemap, VkImageViewType view_type, VkImageAspectFlags aspect, VkFilter mag_filter, VkFilter min_filter, VkSamplerMipmapMode mipmap_mode, VkSamplerAddressMode u_address_mode, VkSamplerAddressMode v_address_mode, VkSamplerAddressMode w_address_mode, float lod_bias, bool anisotropy_enable, float max_anisotropy, bool compare_enable, VkCompareOp compare_operator, float min_lod, float max_lod, VkBorderColor border_color, bool unnormalized_coords, VkSampler & sampler, VkImage & sampled_image, VkDeviceMemory & memory_object, VkImageView & sampled_image_view) {
		if (!CreateSampler(logical_device, mag_filter, min_filter, mipmap_mode, u_address_mode, v_address_mode, w_address_mode, lod_bias, anisotropy_enable, max_anisotropy, compare_enable, compare_operator, min_lod, max_lod, border_color, unnormalized_coords, sampler)) {
			return false;
		}

		bool linear_filtering = (mag_filter == VK_FILTER_LINEAR) || (min_filter == VK_FILTER_LINEAR) || (mipmap_mode == VK_SAMPLER_MIPMAP_MODE_LINEAR);
		if (!CreateSampledImage(physical_device, logical_device, type, format, size, num_mipmaps, num_layers, usage, cubemap, view_type, aspect, linear_filtering, sampled_image, memory_object, sampled_image_view)) {
			return false;
		}
		return true;
	}

	bool CreateStorageImage(VkPhysicalDevice physical_device, VkDevice logical_device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t num_mipmaps, uint32_t num_layers, VkImageUsageFlags usage, VkImageViewType view_type, VkImageAspectFlags aspect, bool atomic_operations, VkImage & storage_image, VkDeviceMemory & memory_object, VkImageView & storage_image_view) {
		VkFormatProperties format_properties;
		vkGetPhysicalDeviceFormatProperties(physical_device, format, &format_properties);
		if (!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT)) {
			LOG( "Provided format is not supported for a storage image." );
			return false;
		}
		if (atomic_operations &&
			!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT)) {
			LOG( "Provided format is not supported for atomic operations on storage images." );
			return false;
		}

		if (!CreateImage(logical_device, type, format, size, num_mipmaps, num_layers, VK_SAMPLE_COUNT_1_BIT, usage | VK_IMAGE_USAGE_STORAGE_BIT, false, storage_image)) {
			return false;
		}

		if (!AllocateAndBindMemoryObjectToImage(physical_device, logical_device, storage_image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memory_object)) {
			return false;
		}

		if (!CreateImageView(logical_device, storage_image, view_type, format, aspect, storage_image_view)) {
			return false;
		}
		return true;
	}

	bool CreateUniformTexelBuffer(VkPhysicalDevice physical_device, VkDevice logical_device, VkFormat format, VkDeviceSize size, VkImageUsageFlags usage, VkBuffer & uniform_texel_buffer, VkDeviceMemory & memory_object, VkBufferView & uniform_texel_buffer_view) {
		VkFormatProperties format_properties;
		vkGetPhysicalDeviceFormatProperties(physical_device, format, &format_properties);
		if (!(format_properties.bufferFeatures & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT)) {
			LOG( "Provided format is not supported for a uniform texel buffer." );
			return false;
		}

		if (!CreateBuffer(logical_device, size, usage | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT, uniform_texel_buffer)) {
			return false;
		}

		if (!AllocateAndBindMemoryObjectToBuffer(physical_device, logical_device, uniform_texel_buffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memory_object)) {
			return false;
		}

		if (!CreateBufferView(logical_device, uniform_texel_buffer, format, 0, VK_WHOLE_SIZE, uniform_texel_buffer_view)) {
			return false;
		}
		return true;
	}

	bool CreateStorageTexelBuffer(VkPhysicalDevice physical_device, VkDevice logical_device, VkFormat format, VkDeviceSize size, VkBufferUsageFlags usage, bool atomic_operations, VkBuffer & storage_texel_buffer, VkDeviceMemory & memory_object, VkBufferView & storage_texel_buffer_view) {
		VkFormatProperties format_properties;
		vkGetPhysicalDeviceFormatProperties(physical_device, format, &format_properties);
		if (!(format_properties.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT)) {
			LOG( "Provided format is not supported for a uniform texel buffer." );
			return false;
		}

		if (atomic_operations &&
			!(format_properties.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT)) {
			LOG( "Provided format is not supported for atomic operations on storage texel buffers." );
			return false;
		}

		if (!CreateBuffer(logical_device, size, usage | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT, storage_texel_buffer)) {
			return false;
		}

		if (!AllocateAndBindMemoryObjectToBuffer(physical_device, logical_device, storage_texel_buffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memory_object)) {
			return false;
		}

		if (!CreateBufferView(logical_device, storage_texel_buffer, format, 0, VK_WHOLE_SIZE, storage_texel_buffer_view)) {
			return false;
		}
		return true;
	}

	bool CreateUniformBuffer(VkPhysicalDevice physical_device, VkDevice logical_device, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer & uniform_buffer, VkDeviceMemory & memory_object) {
		if (!CreateBuffer(logical_device, size, usage | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, uniform_buffer)) {
			return false;
		}

		if (!AllocateAndBindMemoryObjectToBuffer(physical_device, logical_device, uniform_buffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memory_object)) {
			return false;
		}
		return true;
	}

	bool CreateStorageBuffer(VkPhysicalDevice physical_device, VkDevice logical_device, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer & storage_buffer, VkDeviceMemory & memory_object) {
		if (!CreateBuffer(logical_device, size, usage | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, storage_buffer)) {
			return false;
		}

		if (!AllocateAndBindMemoryObjectToBuffer(physical_device, logical_device, storage_buffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memory_object)) {
			return false;
		}
		return true;
	}

	bool CreateInputAttachment(VkPhysicalDevice physical_device, VkDevice logical_device, VkImageType type, VkFormat format, VkExtent3D size, VkImageUsageFlags usage, VkImageViewType view_type, VkImageAspectFlags aspect, VkImage & input_attachment, VkDeviceMemory & memory_object, VkImageView & input_attachment_image_view) {
		VkFormatProperties format_properties;
		vkGetPhysicalDeviceFormatProperties(physical_device, format, &format_properties);
		if ((aspect & VK_IMAGE_ASPECT_COLOR_BIT) &&
			!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT)) {
			LOG( "Provided format is not supported for an input attachment." );
			return false;
		}
		if ((aspect & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) &&
			!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)) {
			LOG( "Provided format is not supported for an input attachment." );
			return false;
		}

		if (!CreateImage(logical_device, type, format, size, 1, 1, VK_SAMPLE_COUNT_1_BIT, usage | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, false, input_attachment)) {
			return false;
		}

		if (!AllocateAndBindMemoryObjectToImage(physical_device, logical_device, input_attachment, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memory_object)) {
			return false;
		}

		if (!CreateImageView(logical_device, input_attachment, view_type, format, aspect, input_attachment_image_view)) {
			return false;
		}
		return true;
	}

	bool CreateDescriptorSetLayout(VkDevice logical_device, std::vector<VkDescriptorSetLayoutBinding> const & bindings, VkDescriptorSetLayout & descriptor_set_layout) {
		VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,  // VkStructureType                      sType
			nullptr,                                              // const void                         * pNext
			0,                                                    // VkDescriptorSetLayoutCreateFlags     flags
			static_cast<uint32_t>(bindings.size()),               // uint32_t                             bindingCount
			bindings.data()                                       // const VkDescriptorSetLayoutBinding * pBindings
		};

		VkResult result = vkCreateDescriptorSetLayout(logical_device, &descriptor_set_layout_create_info, nullptr, &descriptor_set_layout);
		if (VK_SUCCESS != result) {
			LOG( "Could not create a layout for descriptor sets." );
			return false;
		}
		return true;
	}

	bool CreateDescriptorPool(VkDevice logical_device, bool free_individual_sets, uint32_t max_sets_count, std::vector<VkDescriptorPoolSize> const & descriptor_types, VkDescriptorPool & descriptor_pool) {
		VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,                // VkStructureType                sType
			nullptr,                                                      // const void                   * pNext
			free_individual_sets ?                                        // VkDescriptorPoolCreateFlags    flags
			VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT : 0u,
			max_sets_count,                                               // uint32_t                       maxSets
			static_cast<uint32_t>(descriptor_types.size()),               // uint32_t                       poolSizeCount
			descriptor_types.data()                                       // const VkDescriptorPoolSize   * pPoolSizes
		};

		VkResult result = vkCreateDescriptorPool(logical_device, &descriptor_pool_create_info, nullptr, &descriptor_pool);
		if (VK_SUCCESS != result) {
			LOG( "Could not create a descriptor pool." );
			return false;
		}
		return true;
	}

	bool AllocateDescriptorSets(VkDevice logical_device, VkDescriptorPool descriptor_pool, std::vector<VkDescriptorSetLayout> const & descriptor_set_layouts, std::vector<VkDescriptorSet> & descriptor_sets) {
		if (descriptor_set_layouts.size() > 0) {
			VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
				VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,         // VkStructureType                  sType
				nullptr,                                                // const void                     * pNext
				descriptor_pool,                                        // VkDescriptorPool                 descriptorPool
				static_cast<uint32_t>(descriptor_set_layouts.size()),   // uint32_t                         descriptorSetCount
				descriptor_set_layouts.data()                           // const VkDescriptorSetLayout    * pSetLayouts
			};

			descriptor_sets.resize(descriptor_set_layouts.size());

			VkResult result = vkAllocateDescriptorSets(logical_device, &descriptor_set_allocate_info, descriptor_sets.data());
			if (VK_SUCCESS != result) {
				LOG( "Could not allocate descriptor sets." );
				return false;
			}
			return true;
		}
		return false;
	}


	void UpdateDescriptorSets(VkDevice logical_device, std::vector<ImageDescriptorInfo> const & image_descriptor_infos, std::vector<BufferDescriptorInfo> const & buffer_descriptor_infos, std::vector<TexelBufferDescriptorInfo> const & texel_buffer_descriptor_infos, std::vector<CopyDescriptorInfo> const & copy_descriptor_infos) {
		std::vector<VkWriteDescriptorSet> write_descriptors;
		std::vector<VkCopyDescriptorSet> copy_descriptors;

		// image descriptors
		for (auto & image_descriptor : image_descriptor_infos) {
			write_descriptors.push_back({
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,                                 // VkStructureType                  sType
				nullptr,                                                                // const void                     * pNext
				image_descriptor.TargetDescriptorSet,                                   // VkDescriptorSet                  dstSet
				image_descriptor.TargetDescriptorBinding,                               // uint32_t                         dstBinding
				image_descriptor.TargetArrayElement,                                    // uint32_t                         dstArrayElement
				static_cast<uint32_t>(image_descriptor.ImageInfos.size()),              // uint32_t                         descriptorCount
				image_descriptor.TargetDescriptorType,                                  // VkDescriptorType                 descriptorType
				image_descriptor.ImageInfos.data(),                                     // const VkDescriptorImageInfo    * pImageInfo
				nullptr,                                                                // const VkDescriptorBufferInfo   * pBufferInfo
				nullptr                                                                 // const VkBufferView             * pTexelBufferView
				});
		}

		// buffer descriptors
		for (auto & buffer_descriptor : buffer_descriptor_infos) {
			write_descriptors.push_back({
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,                                 // VkStructureType                  sType
				nullptr,                                                                // const void                     * pNext
				buffer_descriptor.TargetDescriptorSet,                                  // VkDescriptorSet                  dstSet
				buffer_descriptor.TargetDescriptorBinding,                              // uint32_t                         dstBinding
				buffer_descriptor.TargetArrayElement,                                   // uint32_t                         dstArrayElement
				static_cast<uint32_t>(buffer_descriptor.BufferInfos.size()),            // uint32_t                         descriptorCount
				buffer_descriptor.TargetDescriptorType,                                 // VkDescriptorType                 descriptorType
				nullptr,                                                                // const VkDescriptorImageInfo    * pImageInfo
				buffer_descriptor.BufferInfos.data(),                                   // const VkDescriptorBufferInfo   * pBufferInfo
				nullptr                                                                 // const VkBufferView             * pTexelBufferView
				});
		}

		// texel buffer descriptors
		for (auto & texel_buffer_descriptor : texel_buffer_descriptor_infos) {
			write_descriptors.push_back({
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,                                 // VkStructureType                  sType
				nullptr,                                                                // const void                     * pNext
				texel_buffer_descriptor.TargetDescriptorSet,                            // VkDescriptorSet                  dstSet
				texel_buffer_descriptor.TargetDescriptorBinding,                        // uint32_t                         dstBinding
				texel_buffer_descriptor.TargetArrayElement,                             // uint32_t                         dstArrayElement
				static_cast<uint32_t>(texel_buffer_descriptor.TexelBufferViews.size()), // uint32_t                         descriptorCount
				texel_buffer_descriptor.TargetDescriptorType,                           // VkDescriptorType                 descriptorType
				nullptr,                                                                // const VkDescriptorImageInfo    * pImageInfo
				nullptr,                                                                // const VkDescriptorBufferInfo   * pBufferInfo
				texel_buffer_descriptor.TexelBufferViews.data()                         // const VkBufferView             * pTexelBufferView
				});
		}

		// copy descriptors
		for (auto & copy_descriptor : copy_descriptor_infos) {
			copy_descriptors.push_back({
				VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET,                                  // VkStructureType    sType
				nullptr,                                                                // const void       * pNext
				copy_descriptor.SourceDescriptorSet,                                    // VkDescriptorSet    srcSet
				copy_descriptor.SourceDescriptorBinding,                                // uint32_t           srcBinding
				copy_descriptor.SourceArrayElement,                                     // uint32_t           srcArrayElement
				copy_descriptor.TargetDescriptorSet,                                    // VkDescriptorSet    dstSet
				copy_descriptor.TargetDescriptorBinding,                                // uint32_t           dstBinding
				copy_descriptor.TargetArrayElement,                                     // uint32_t           dstArrayElement
				copy_descriptor.DescriptorCount                                         // uint32_t           descriptorCount
				});
		}

		vkUpdateDescriptorSets(logical_device, static_cast<uint32_t>(write_descriptors.size()), write_descriptors.data(), static_cast<uint32_t>(copy_descriptors.size()), copy_descriptors.data());
	}

	void BindDescriptorSets(VkCommandBuffer command_buffer, VkPipelineBindPoint pipeline_type, VkPipelineLayout pipeline_layout, uint32_t index_for_first_set, std::vector<VkDescriptorSet> const & descriptor_sets, std::vector<uint32_t> const & dynamic_offsets) {
		vkCmdBindDescriptorSets(command_buffer, pipeline_type, pipeline_layout, index_for_first_set,
		static_cast<uint32_t>(descriptor_sets.size()), descriptor_sets.data(),
		static_cast<uint32_t>(dynamic_offsets.size()), dynamic_offsets.data());
	}

	bool CreateDescriptorsWithTextureAndUniformBuffer(VkPhysicalDevice physical_device, VkDevice logical_device, VkExtent3D sampled_image_size, uint32_t uniform_buffer_size, VkSampler & sampler, VkImage & sampled_image, VkDeviceMemory & sampled_image_memory_object, VkImageView & sampled_image_view, VkBuffer & uniform_buffer, VkDeviceMemory & uniform_buffer_memory_object, VkDescriptorSetLayout & descriptor_set_layout, VkDescriptorPool & descriptor_pool, std::vector<VkDescriptorSet> & descriptor_sets) {
		if (!CreateCombinedImageSampler(physical_device, logical_device, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, sampled_image_size, 1, 1, VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			false, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT,
			VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT, 0.0f, false, 1.0f, false, VK_COMPARE_OP_ALWAYS, 0.0f, 0.0f, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, false,
			sampler, sampled_image, sampled_image_memory_object, sampled_image_view)) {
			return false;
		}

		if (!CreateUniformBuffer(physical_device, logical_device, uniform_buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT, uniform_buffer, uniform_buffer_memory_object)) {
			return false;
		}

		std::vector<VkDescriptorSetLayoutBinding> bindings = {
			{
				0,                                                          // uint32_t             binding
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,                  // VkDescriptorType     descriptorType
				1,                                                          // uint32_t             descriptorCount
				VK_SHADER_STAGE_FRAGMENT_BIT,                               // VkShaderStageFlags   stageFlags
				nullptr                                                     // const VkSampler    * pImmutableSamplers
			},
			{
				1,                                                          // uint32_t             binding
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,                          // VkDescriptorType     descriptorType
				1,                                                          // uint32_t             descriptorCount
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,  // VkShaderStageFlags   stageFlags
				nullptr                                                     // const VkSampler    * pImmutableSamplers
			}
		};
		if (!CreateDescriptorSetLayout(logical_device, bindings, descriptor_set_layout)) {
			return false;
		}

		std::vector<VkDescriptorPoolSize> descriptor_types = {
			{
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,                  // VkDescriptorType     type
				1                                                           // uint32_t             descriptorCount
			},
			{
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,                          // VkDescriptorType     type
				1                                                           // uint32_t             descriptorCount
			}
		};
		if (!CreateDescriptorPool(logical_device, false, 1, descriptor_types, descriptor_pool)) {
			return false;
		}

		if (!AllocateDescriptorSets(logical_device, descriptor_pool, { descriptor_set_layout }, descriptor_sets)) {
			return false;
		}

		std::vector<ImageDescriptorInfo> image_descriptor_infos = {
			{
				descriptor_sets[0],                         // VkDescriptorSet                      TargetDescriptorSet
				0,                                          // uint32_t                             TargetDescriptorBinding
				0,                                          // uint32_t                             TargetArrayElement
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // VkDescriptorType                     TargetDescriptorType
				{                                           // std::vector<VkDescriptorImageInfo>   ImageInfos
					{
						sampler,                                  // VkSampler                            sampler
						sampled_image_view,                       // VkImageView                          imageView
						VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL  // VkImageLayout                        imageLayout
					}
				}
			}
		};

		std::vector<BufferDescriptorInfo> buffer_descriptor_infos = {
			{
				descriptor_sets[0],                         // VkDescriptorSet                      TargetDescriptorSet
				1,                                          // uint32_t                             TargetDescriptorBinding
				0,                                          // uint32_t                             TargetArrayElement
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          // VkDescriptorType                     TargetDescriptorType
				{                                           // std::vector<VkDescriptorBufferInfo>  BufferInfos
					{
						uniform_buffer,                           // VkBuffer                             buffer
						0,                                        // VkDeviceSize                         offset
						VK_WHOLE_SIZE                             // VkDeviceSize                         range
					}
				}
			}
		};

		UpdateDescriptorSets(logical_device, image_descriptor_infos, buffer_descriptor_infos, {}, {});
		return true;
	}

	bool FreeDescriptorSets(VkDevice logical_device, VkDescriptorPool descriptor_pool, std::vector<VkDescriptorSet> & descriptor_sets) {
		if (descriptor_sets.size() > 0) {
			VkResult result = vkFreeDescriptorSets(logical_device, descriptor_pool, static_cast<uint32_t>(descriptor_sets.size()), descriptor_sets.data());
			descriptor_sets.clear();
			if (VK_SUCCESS != result) {
				LOG( "Error occurred during freeing descriptor sets." );
				return false;
			}
		}
		return true;
	}

	bool ResetDescriptorPool(VkDevice logical_device, VkDescriptorPool descriptor_pool) {
		VkResult result = vkResetDescriptorPool(logical_device, descriptor_pool, 0);
		if (VK_SUCCESS != result) {
			LOG( "Error occurred during descriptor pool reset." );
			return false;
		}
		return true;
	}

	void DestroyDescriptorPool(VkDevice logical_device, VkDescriptorPool & descriptor_pool) {
		if (VK_NULL_HANDLE != descriptor_pool) {
			vkDestroyDescriptorPool(logical_device, descriptor_pool, nullptr);
			descriptor_pool = VK_NULL_HANDLE;
		}
	}

	void DestroyDescriptorSetLayout(VkDevice logical_device, VkDescriptorSetLayout & descriptor_set_layout) {
		if (VK_NULL_HANDLE != descriptor_set_layout) {
			vkDestroyDescriptorSetLayout(logical_device, descriptor_set_layout, nullptr);
			descriptor_set_layout = VK_NULL_HANDLE;
		}
	}

	void DestroySampler(VkDevice logical_device, VkSampler & sampler) {
		if (VK_NULL_HANDLE != sampler) {
			vkDestroySampler(logical_device, sampler, nullptr);
			sampler = VK_NULL_HANDLE;
		}
	}

	void SpecifyAttachmentsDescriptions(std::vector<VkAttachmentDescription> const & attachments_descriptions) {
		// typedef struct VkAttachmentDescription {
		//   VkAttachmentDescriptionFlags    flags;
		//   VkFormat                        format;
		//   VkSampleCountFlagBits           samples;
		//   VkAttachmentLoadOp              loadOp;
		//   VkAttachmentStoreOp             storeOp;
		//   VkAttachmentLoadOp              stencilLoadOp;
		//   VkAttachmentStoreOp             stencilStoreOp;
		//   VkImageLayout                   initialLayout;
		//   VkImageLayout                   finalLayout;
		// } VkAttachmentDescription;
	}

	void SpecifySubpassDescriptions(std::vector<SubpassParameters> const & subpass_parameters, std::vector<VkSubpassDescription> & subpass_descriptions) {
		subpass_descriptions.clear();

		for (auto & subpass_description : subpass_parameters) {
			subpass_descriptions.push_back({
				0,                                                                      // VkSubpassDescriptionFlags        flags
				subpass_description.PipelineType,                                       // VkPipelineBindPoint              pipelineBindPoint
				static_cast<uint32_t>(subpass_description.InputAttachments.size()),     // uint32_t                         inputAttachmentCount
				subpass_description.InputAttachments.data(),                            // const VkAttachmentReference    * pInputAttachments
				static_cast<uint32_t>(subpass_description.ColorAttachments.size()),     // uint32_t                         colorAttachmentCount
				subpass_description.ColorAttachments.data(),                            // const VkAttachmentReference    * pColorAttachments
				subpass_description.ResolveAttachments.data(),                          // const VkAttachmentReference    * pResolveAttachments
				subpass_description.DepthStencilAttachment,                             // const VkAttachmentReference    * pDepthStencilAttachment
				static_cast<uint32_t>(subpass_description.PreserveAttachments.size()),  // uint32_t                         preserveAttachmentCount
				subpass_description.PreserveAttachments.data()                          // const uint32_t                 * pPreserveAttachments
				});
		}
	}

	void SpecifyDependenciesBetweenSubpasses(std::vector<VkSubpassDependency> const & subpasses_dependencies) {
		// typedef struct VkSubpassDependency {
		//   uint32_t                srcSubpass;
		//   uint32_t                dstSubpass;
		//   VkPipelineStageFlags    srcStageMask;
		//   VkPipelineStageFlags    dstStageMask;
		//   VkAccessFlags           srcAccessMask;
		//   VkAccessFlags           dstAccessMask;
		//   VkDependencyFlags       dependencyFlags;
		// } VkSubpassDependency;
	}

	bool CreateRenderPass(VkDevice logical_device, std::vector<VkAttachmentDescription> const & attachments_descriptions, std::vector<SubpassParameters> const & subpass_parameters, std::vector<VkSubpassDependency> const & subpass_dependencies, VkRenderPass & render_pass) {
		SpecifyAttachmentsDescriptions(attachments_descriptions);

		std::vector<VkSubpassDescription> subpass_descriptions;
		SpecifySubpassDescriptions(subpass_parameters, subpass_descriptions);

		SpecifyDependenciesBetweenSubpasses(subpass_dependencies);

		VkRenderPassCreateInfo render_pass_create_info = {
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,                // VkStructureType                    sType
			nullptr,                                                  // const void                       * pNext
			0,                                                        // VkRenderPassCreateFlags            flags
			static_cast<uint32_t>(attachments_descriptions.size()),   // uint32_t                           attachmentCount
			attachments_descriptions.data(),                          // const VkAttachmentDescription    * pAttachments
			static_cast<uint32_t>(subpass_descriptions.size()),       // uint32_t                           subpassCount
			subpass_descriptions.data(),                              // const VkSubpassDescription       * pSubpasses
			static_cast<uint32_t>(subpass_dependencies.size()),       // uint32_t                           dependencyCount
			subpass_dependencies.data()                               // const VkSubpassDependency        * pDependencies
		};

		VkResult result = vkCreateRenderPass(logical_device, &render_pass_create_info, nullptr, &render_pass);
		if (VK_SUCCESS != result) {
			LOG( "Could not create a render pass." );
			return false;
		}
		return true;
	}

	bool CreateFramebuffer(VkDevice logical_device, VkRenderPass render_pass, std::vector<VkImageView> const & attachments, uint32_t width, uint32_t height, uint32_t layers, VkFramebuffer & framebuffer) {
		VkFramebufferCreateInfo framebuffer_create_info = {
			VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,    // VkStructureType              sType
			nullptr,                                      // const void                 * pNext
			0,                                            // VkFramebufferCreateFlags     flags
			render_pass,                                  // VkRenderPass                 renderPass
			static_cast<uint32_t>(attachments.size()),    // uint32_t                     attachmentCount
			attachments.data(),                           // const VkImageView          * pAttachments
			width,                                        // uint32_t                     width
			height,                                       // uint32_t                     height
			layers                                        // uint32_t                     layers
		};

		VkResult result = vkCreateFramebuffer(logical_device, &framebuffer_create_info, nullptr, &framebuffer);
		if (VK_SUCCESS != result) {
			LOG( "Could not create a framebuffer." );
			return false;
		}
		return true;
	}

	bool PrepareRenderPassForGeometryRenderingAndPostprocessSubpasses(VkDevice logical_device, VkRenderPass render_pass) {
		std::vector<VkAttachmentDescription> attachments_descriptions = {
			{
				0,                                                // VkAttachmentDescriptionFlags     flags
				VK_FORMAT_R8G8B8A8_UNORM,                         // VkFormat                         format
				VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits            samples
				VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp               loadOp
				VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              storeOp
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               stencilLoadOp
				VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              stencilStoreOp
				VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                    initialLayout
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,         // VkImageLayout                    finalLayout
			},
			{
				0,                                                // VkAttachmentDescriptionFlags     flags
				VK_FORMAT_D16_UNORM,                              // VkFormat                         format
				VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits            samples
				VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp               loadOp
				VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              storeOp
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               stencilLoadOp
				VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              stencilStoreOp
				VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                    initialLayout
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, // VkImageLayout                    finalLayout
			},
			{
				0,                                                // VkAttachmentDescriptionFlags     flags
				VK_FORMAT_R8G8B8A8_UNORM,                         // VkFormat                         format
				VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits            samples
				VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp               loadOp
				VK_ATTACHMENT_STORE_OP_STORE,                     // VkAttachmentStoreOp              storeOp
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               stencilLoadOp
				VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              stencilStoreOp
				VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                    initialLayout
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                  // VkImageLayout                    finalLayout
			},
		};

		VkAttachmentReference depth_stencil_attachment = {
			1,                                                  // uint32_t                             attachment
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,   // VkImageLayout                        layout
		};

		std::vector<SubpassParameters> subpass_parameters = {
			// #0 subpass
			{
				VK_PIPELINE_BIND_POINT_GRAPHICS,                  // VkPipelineBindPoint                  PipelineType
				{},                                               // std::vector<VkAttachmentReference>   InputAttachments
				{                                                 // std::vector<VkAttachmentReference>   ColorAttachments
					{
						0,                                        // uint32_t                             attachment
						VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL  // VkImageLayout                        layout
					}
				},
				{},                                               // std::vector<VkAttachmentReference>   ResolveAttachments
				&depth_stencil_attachment,                        // const VkAttachmentReference        * DepthStencilAttachment
				{}                                                // std::vector<uint32_t>                PreserveAttachments
			},
			// #1 subpass
			{
				VK_PIPELINE_BIND_POINT_GRAPHICS,                  // VkPipelineBindPoint                  PipelineType
				{                                                 // std::vector<VkAttachmentReference>   InputAttachments
					{
						0,                                        // uint32_t                             attachment
						VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL  // VkImageLayout                        layout
					}
				},
				{                                                 // std::vector<VkAttachmentReference>   ColorAttachments
					{
						2,                                        // uint32_t                             attachment
						VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL  // VkImageLayout                        layout
					}
				},
				{},                                               // std::vector<VkAttachmentReference>   ResolveAttachments
				nullptr,                                          // const VkAttachmentReference        * DepthStencilAttachment
				{}                                                // std::vector<uint32_t>                PreserveAttachments
			}
		};

		std::vector<VkSubpassDependency> subpass_dependencies = {
			{
				0,                                              // uint32_t                 srcSubpass
				1,                                              // uint32_t                 dstSubpass
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // VkPipelineStageFlags     srcStageMask
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,          // VkPipelineStageFlags     dstStageMask
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           // VkAccessFlags            srcAccessMask
				VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,            // VkAccessFlags            dstAccessMask
				VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags        dependencyFlags
			}
		};

		if (!CreateRenderPass(logical_device, attachments_descriptions, subpass_parameters, subpass_dependencies, render_pass)) {
			return false;
		}
		return true;
	}

	bool PrepareRenderPassAndFramebufferWithColorAndDepthAttachments(VkPhysicalDevice physical_device, VkDevice logical_device, uint32_t width, uint32_t height, VkImage & color_image, VkDeviceMemory & color_image_memory_object, VkImageView & color_image_view, VkImage & depth_image, VkDeviceMemory & depth_image_memory_object, VkImageView & depth_image_view, VkRenderPass & render_pass, VkFramebuffer & framebuffer) {
		if (!Create2DImageAndView(physical_device, logical_device, VK_FORMAT_R8G8B8A8_UNORM, { width, height }, 1, 1, VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, color_image, color_image_memory_object, color_image_view)) {
			return false;
		}

		if (!Create2DImageAndView(physical_device, logical_device, VK_FORMAT_D16_UNORM, { width, height }, 1, 1, VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, depth_image, depth_image_memory_object, depth_image_view)) {
			return false;
		}

		std::vector<VkAttachmentDescription> attachments_descriptions = {
			{
				0,                                                // VkAttachmentDescriptionFlags     flags
				VK_FORMAT_R8G8B8A8_UNORM,                         // VkFormat                         format
				VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits            samples
				VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp               loadOp
				VK_ATTACHMENT_STORE_OP_STORE,                     // VkAttachmentStoreOp              storeOp
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               stencilLoadOp
				VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              stencilStoreOp
				VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                    initialLayout
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,         // VkImageLayout                    finalLayout
			},
			{
				0,                                                // VkAttachmentDescriptionFlags     flags
				VK_FORMAT_D16_UNORM,                              // VkFormat                         format
				VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits            samples
				VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp               loadOp
				VK_ATTACHMENT_STORE_OP_STORE,                     // VkAttachmentStoreOp              storeOp
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               stencilLoadOp
				VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              stencilStoreOp
				VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                    initialLayout
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,  // VkImageLayout                    finalLayout
			}
		};

		VkAttachmentReference depth_stencil_attachment = {
			1,                                                  // uint32_t                             attachment
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,   // VkImageLayout                        layout
		};

		std::vector<SubpassParameters> subpass_parameters = {
			{
				VK_PIPELINE_BIND_POINT_GRAPHICS,                  // VkPipelineBindPoint                  PipelineType
				{},                                               // std::vector<VkAttachmentReference>   InputAttachments
				{                                                 // std::vector<VkAttachmentReference>   ColorAttachments
					{
						0,                                        // uint32_t                             attachment
						VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL  // VkImageLayout                        layout
					}
				},
				{},                                               // std::vector<VkAttachmentReference>   ResolveAttachments
				&depth_stencil_attachment,                        // const VkAttachmentReference        * DepthStencilAttachment
				{}                                                // std::vector<uint32_t>                PreserveAttachments
			}
		};

		std::vector<VkSubpassDependency> subpass_dependencies = {
			{
				0,                                                // uint32_t                 srcSubpass
				VK_SUBPASS_EXTERNAL,                              // uint32_t                 dstSubpass
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,    // VkPipelineStageFlags     srcStageMask
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,            // VkPipelineStageFlags     dstStageMask
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,             // VkAccessFlags            srcAccessMask
				VK_ACCESS_SHADER_READ_BIT,                        // VkAccessFlags            dstAccessMask
				0                                                 // VkDependencyFlags        dependencyFlags
			}
		};

		if (!CreateRenderPass(logical_device, attachments_descriptions, subpass_parameters, subpass_dependencies, render_pass)) {
			return false;
		}

		if (!CreateFramebuffer(logical_device, render_pass, { color_image_view, depth_image_view }, width, height, 1, framebuffer)) {
			return false;
		}
		return true;
	}

	void BeginRenderPass(VkCommandBuffer command_buffer, VkRenderPass render_pass, VkFramebuffer framebuffer, VkRect2D render_area, std::vector<VkClearValue> const & clear_values, VkSubpassContents subpass_contents) {
		VkRenderPassBeginInfo render_pass_begin_info = {
			VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,     // VkStructureType        sType
			nullptr,                                      // const void           * pNext
			render_pass,                                  // VkRenderPass           renderPass
			framebuffer,                                  // VkFramebuffer          framebuffer
			render_area,                                  // VkRect2D               renderArea
			static_cast<uint32_t>(clear_values.size()),   // uint32_t               clearValueCount
			clear_values.data()                           // const VkClearValue   * pClearValues
		};

		vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, subpass_contents);
	}

	void ProgressToTheNextSubpass(VkCommandBuffer command_buffer, VkSubpassContents subpass_contents) {
		vkCmdNextSubpass(command_buffer, subpass_contents);
	}

	void EndRenderPass(VkCommandBuffer command_buffer) {
		vkCmdEndRenderPass(command_buffer);
	}

	void DestroyFramebuffer(VkDevice logical_device, VkFramebuffer & framebuffer) {
		if (VK_NULL_HANDLE != framebuffer) {
			vkDestroyFramebuffer(logical_device, framebuffer, nullptr);
			framebuffer = VK_NULL_HANDLE;
		}
	}

	void DestroyRenderPass(VkDevice logical_device, VkRenderPass & render_pass) {
		if (VK_NULL_HANDLE != render_pass) {
			vkDestroyRenderPass(logical_device, render_pass, nullptr);
			render_pass = VK_NULL_HANDLE;
		}
	}

	bool CreateShaderModule(VkDevice logical_device, std::vector<unsigned char> const & source_code, VkShaderModule & shader_module) {
		VkShaderModuleCreateInfo shader_module_create_info = {
			VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,              // VkStructureType              sType
			nullptr,                                                  // const void                 * pNext
			0,                                                        // VkShaderModuleCreateFlags    flags
			source_code.size(),                                       // size_t                       codeSize
			reinterpret_cast<uint32_t const *>(source_code.data())    // const uint32_t             * pCode
		};

		VkResult result = vkCreateShaderModule(logical_device, &shader_module_create_info, nullptr, &shader_module);
		if (VK_SUCCESS != result) {
			LOG( "Could not create a shader module." );
			return false;
		}
		return true;
	}

	void SpecifyPipelineShaderStages(std::vector<ShaderStageParameters> const & shader_stage_params, std::vector<VkPipelineShaderStageCreateInfo> & shader_stage_create_infos) {
		shader_stage_create_infos.clear();
		for (auto & shader_stage : shader_stage_params) {
			shader_stage_create_infos.push_back({
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,  // VkStructureType                    sType
				nullptr,                                              // const void                       * pNext
				0,                                                    // VkPipelineShaderStageCreateFlags   flags
				shader_stage.ShaderStage,                             // VkShaderStageFlagBits              stage
				shader_stage.ShaderModule,                            // VkShaderModule                     module
				shader_stage.EntryPointName,                          // const char                       * pName
				shader_stage.SpecializationInfo                       // const VkSpecializationInfo       * pSpecializationInfo
			});
		}
	}

	void SpecifyPipelineVertexInputState(std::vector<VkVertexInputBindingDescription> const & binding_descriptions, std::vector<VkVertexInputAttributeDescription> const & attribute_descriptions, VkPipelineVertexInputStateCreateInfo & vertex_input_state_create_info) {
		vertex_input_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,    // VkStructureType                           sType
			nullptr,                                                      // const void                              * pNext
			0,                                                            // VkPipelineVertexInputStateCreateFlags     flags
			static_cast<uint32_t>(binding_descriptions.size()),           // uint32_t                                  vertexBindingDescriptionCount
			binding_descriptions.data(),                                  // const VkVertexInputBindingDescription   * pVertexBindingDescriptions
			static_cast<uint32_t>(attribute_descriptions.size()),         // uint32_t                                  vertexAttributeDescriptionCount
			attribute_descriptions.data()                                 // const VkVertexInputAttributeDescription * pVertexAttributeDescriptions
		};
	}

	void SpecifyPipelineInputAssemblyState(VkPrimitiveTopology topology, bool primitive_restart_enable, VkPipelineInputAssemblyStateCreateInfo & input_assembly_state_create_info) {
		input_assembly_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,  // VkStructureType                           sType
			nullptr,                                                      // const void                              * pNext
			0,                                                            // VkPipelineInputAssemblyStateCreateFlags   flags
			topology,                                                     // VkPrimitiveTopology                       topology
			primitive_restart_enable                                      // VkBool32                                  primitiveRestartEnable
		};
	}

	void SpecifyPipelineTessellationState(uint32_t patch_control_points_count, VkPipelineTessellationStateCreateInfo & tessellation_state_create_info) {
		tessellation_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,    // VkStructureType                            sType
			nullptr,                                                      // const void                               * pNext
			0,                                                            // VkPipelineTessellationStateCreateFlags     flags
			patch_control_points_count                                    // uint32_t                                   patchControlPoints
		};
	}

	void SpecifyPipelineViewportAndScissorTestState(ViewportInfo const & viewport_infos, VkPipelineViewportStateCreateInfo & viewport_state_create_info) {
		uint32_t viewport_count = static_cast<uint32_t>(viewport_infos.Viewports.size());
		uint32_t scissor_count = static_cast<uint32_t>(viewport_infos.Scissors.size());
		viewport_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,    // VkStructureType                      sType
			nullptr,                                                  // const void                         * pNext
			0,                                                        // VkPipelineViewportStateCreateFlags   flags
			viewport_count,                                           // uint32_t                             viewportCount
			viewport_infos.Viewports.data(),                          // const VkViewport                   * pViewports
			scissor_count,                                            // uint32_t                             scissorCount
			viewport_infos.Scissors.data()                            // const VkRect2D                     * pScissors
		};
	}

	void SpecifyPipelineRasterizationState(bool depth_clamp_enable, bool rasterizer_discard_enable, VkPolygonMode polygon_mode, VkCullModeFlags culling_mode, VkFrontFace front_face, bool depth_bias_enable, float depth_bias_constant_factor, float depth_bias_clamp, float depth_bias_slope_factor, float line_width, VkPipelineRasterizationStateCreateInfo & rasterization_state_create_info) {
		rasterization_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, // VkStructureType                            sType
			nullptr,                                                    // const void                               * pNext
			0,                                                          // VkPipelineRasterizationStateCreateFlags    flags
			depth_clamp_enable,                                         // VkBool32                                   depthClampEnable
			rasterizer_discard_enable,                                  // VkBool32                                   rasterizerDiscardEnable
			polygon_mode,                                               // VkPolygonMode                              polygonMode
			culling_mode,                                               // VkCullModeFlags                            cullMode
			front_face,                                                 // VkFrontFace                                frontFace
			depth_bias_enable,                                          // VkBool32                                   depthBiasEnable
			depth_bias_constant_factor,                                 // float                                      depthBiasConstantFactor
			depth_bias_clamp,                                           // float                                      depthBiasClamp
			depth_bias_slope_factor,                                    // float                                      depthBiasSlopeFactor
			line_width                                                  // float                                      lineWidth
		};
	}

	void SpecifyPipelineMultisampleState(VkSampleCountFlagBits sample_count, bool per_sample_shading_enable, float min_sample_shading, VkSampleMask const * sample_masks, bool alpha_to_coverage_enable, bool alpha_to_one_enable, VkPipelineMultisampleStateCreateInfo & multisample_state_create_info) {
		multisample_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, // VkStructureType                          sType
			nullptr,                                                  // const void                             * pNext
			0,                                                        // VkPipelineMultisampleStateCreateFlags    flags
			sample_count,                                             // VkSampleCountFlagBits                    rasterizationSamples
			per_sample_shading_enable,                                // VkBool32                                 sampleShadingEnable
			min_sample_shading,                                       // float                                    minSampleShading
			sample_masks,                                             // const VkSampleMask                     * pSampleMask
			alpha_to_coverage_enable,                                 // VkBool32                                 alphaToCoverageEnable
			alpha_to_one_enable                                       // VkBool32                                 alphaToOneEnable
		};
	}

	void SpecifyPipelineDepthAndStencilState(bool depth_test_enable, bool depth_write_enable, VkCompareOp depth_compare_op, bool depth_bounds_test_enable, float min_depth_bounds, float max_depth_bounds, bool stencil_test_enable, VkStencilOpState front_stencil_test_parameters, VkStencilOpState back_stencil_test_parameters, VkPipelineDepthStencilStateCreateInfo & depth_and_stencil_state_create_info) {
		depth_and_stencil_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,   // VkStructureType                            sType
			nullptr,                                                      // const void                               * pNext
			0,                                                            // VkPipelineDepthStencilStateCreateFlags     flags
			depth_test_enable,                                            // VkBool32                                   depthTestEnable
			depth_write_enable,                                           // VkBool32                                   depthWriteEnable
			depth_compare_op,                                             // VkCompareOp                                depthCompareOp
			depth_bounds_test_enable,                                     // VkBool32                                   depthBoundsTestEnable
			stencil_test_enable,                                          // VkBool32                                   stencilTestEnable
			front_stencil_test_parameters,                                // VkStencilOpState                           front
			back_stencil_test_parameters,                                 // VkStencilOpState                           back
			min_depth_bounds,                                             // float                                      minDepthBounds
			max_depth_bounds                                              // float                                      maxDepthBounds
		};
	}

	void SpecifyPipelineBlendState(bool logic_op_enable, VkLogicOp logic_op, std::vector<VkPipelineColorBlendAttachmentState> const & attachment_blend_states, std::array<float, 4> const & blend_constants, VkPipelineColorBlendStateCreateInfo & blend_state_create_info) {
		blend_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,   // VkStructureType                              sType
			nullptr,                                                    // const void                                 * pNext
			0,                                                          // VkPipelineColorBlendStateCreateFlags         flags
			logic_op_enable,                                            // VkBool32                                     logicOpEnable
			logic_op,                                                   // VkLogicOp                                    logicOp
			static_cast<uint32_t>(attachment_blend_states.size()),      // uint32_t                                     attachmentCount
			attachment_blend_states.data(),                             // const VkPipelineColorBlendAttachmentState  * pAttachments
			{                                                           // float                                        blendConstants[4]
				blend_constants[0],
				blend_constants[1],
				blend_constants[2],
				blend_constants[3]
			}
		};
	}

	void SpecifyPipelineDynamicStates(std::vector<VkDynamicState> const & dynamic_states, VkPipelineDynamicStateCreateInfo & dynamic_state_creat_info) {
		dynamic_state_creat_info = {
			VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,     // VkStructureType                      sType
			nullptr,                                                  // const void                         * pNext
			0,                                                        // VkPipelineDynamicStateCreateFlags    flags
			static_cast<uint32_t>(dynamic_states.size()),             // uint32_t                             dynamicStateCount
			dynamic_states.data()                                     // const VkDynamicState               * pDynamicStates
		};
	}

	bool CreatePipelineLayout(VkDevice logical_device, std::vector<VkDescriptorSetLayout> const & descriptor_set_layouts, std::vector<VkPushConstantRange> const & push_constant_ranges, VkPipelineLayout & pipeline_layout) {
		VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,          // VkStructureType                  sType
			nullptr,                                                // const void                     * pNext
			0,                                                      // VkPipelineLayoutCreateFlags      flags
			static_cast<uint32_t>(descriptor_set_layouts.size()),   // uint32_t                         setLayoutCount
			descriptor_set_layouts.data(),                          // const VkDescriptorSetLayout    * pSetLayouts
			static_cast<uint32_t>(push_constant_ranges.size()),     // uint32_t                         pushConstantRangeCount
			push_constant_ranges.data()                             // const VkPushConstantRange      * pPushConstantRanges
		};

		VkResult result = vkCreatePipelineLayout(logical_device, &pipeline_layout_create_info, nullptr, &pipeline_layout);

		if (VK_SUCCESS != result) {
			LOG( "Could not create pipeline layout." );
			return false;
		}
		return true;
	}

	void SpecifyGraphicsPipelineCreationParameters(VkPipelineCreateFlags additional_options, std::vector<VkPipelineShaderStageCreateInfo> const & shader_stage_create_infos, VkPipelineVertexInputStateCreateInfo const & vertex_input_state_create_info, VkPipelineInputAssemblyStateCreateInfo const & input_assembly_state_create_info, VkPipelineTessellationStateCreateInfo const * tessellation_state_create_info, VkPipelineViewportStateCreateInfo const * viewport_state_create_info, VkPipelineRasterizationStateCreateInfo const & rasterization_state_create_info, VkPipelineMultisampleStateCreateInfo const * multisample_state_create_info, VkPipelineDepthStencilStateCreateInfo const * depth_and_stencil_state_create_info, VkPipelineColorBlendStateCreateInfo const * blend_state_create_info, VkPipelineDynamicStateCreateInfo const * dynamic_state_creat_info, VkPipelineLayout pipeline_layout, VkRenderPass render_pass, uint32_t subpass, VkPipeline base_pipeline_handle, int32_t base_pipeline_index, VkGraphicsPipelineCreateInfo & graphics_pipeline_create_info) {
		graphics_pipeline_create_info = {
			VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,            // VkStructureType                                sType
			nullptr,                                                    // const void                                   * pNext
			additional_options,                                         // VkPipelineCreateFlags                          flags
			static_cast<uint32_t>(shader_stage_create_infos.size()),    // uint32_t                                       stageCount
			shader_stage_create_infos.data(),                           // const VkPipelineShaderStageCreateInfo        * pStages
			&vertex_input_state_create_info,                            // const VkPipelineVertexInputStateCreateInfo   * pVertexInputState
			&input_assembly_state_create_info,                          // const VkPipelineInputAssemblyStateCreateInfo * pInputAssemblyState
			tessellation_state_create_info,                             // const VkPipelineTessellationStateCreateInfo  * pTessellationState
			viewport_state_create_info,                                 // const VkPipelineViewportStateCreateInfo      * pViewportState
			&rasterization_state_create_info,                           // const VkPipelineRasterizationStateCreateInfo * pRasterizationState
			multisample_state_create_info,                              // const VkPipelineMultisampleStateCreateInfo   * pMultisampleState
			depth_and_stencil_state_create_info,                        // const VkPipelineDepthStencilStateCreateInfo  * pDepthStencilState
			blend_state_create_info,                                    // const VkPipelineColorBlendStateCreateInfo    * pColorBlendState
			dynamic_state_creat_info,                                   // const VkPipelineDynamicStateCreateInfo       * pDynamicState
			pipeline_layout,                                            // VkPipelineLayout                               layout
			render_pass,                                                // VkRenderPass                                   renderPass
			subpass,                                                    // uint32_t                                       subpass
			base_pipeline_handle,                                       // VkPipeline                                     basePipelineHandle
			base_pipeline_index                                         // int32_t                                        basePipelineIndex
		};
	}

	bool CreatePipelineCacheObject(VkDevice logical_device, std::vector<unsigned char> const & cache_data, VkPipelineCache & pipeline_cache) {
		VkPipelineCacheCreateInfo pipeline_cache_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,     // VkStructureType                sType
			nullptr,                                          // const void                   * pNext
			0,                                                // VkPipelineCacheCreateFlags     flags
			static_cast<uint32_t>(cache_data.size()),         // size_t                         initialDataSize
			cache_data.data()                                 // const void                   * pInitialData
		};

		VkResult result = vkCreatePipelineCache(logical_device, &pipeline_cache_create_info, nullptr, &pipeline_cache);
		if (VK_SUCCESS != result) {
			LOG( "Could not create pipeline cache." );
			return false;
		}
		return true;
	}

	bool RetrieveDataFromPipelineCache(VkDevice logical_device, VkPipelineCache pipeline_cache, std::vector<unsigned char> & pipeline_cache_data) {
		size_t data_size = 0;
		VkResult result = VK_SUCCESS;

		result = vkGetPipelineCacheData(logical_device, pipeline_cache, &data_size, nullptr);
		if ((VK_SUCCESS != result) ||
			(0 == data_size)) {
			LOG( "Could not get the size of the pipeline cache." );
			return false;
		}
		pipeline_cache_data.resize(data_size);

		result = vkGetPipelineCacheData(logical_device, pipeline_cache, &data_size, pipeline_cache_data.data());
		if ((VK_SUCCESS != result) ||
			(0 == data_size)) {
			LOG( "Could not acquire pipeline cache data." );
			return false;
		}
		return true;
	}

	bool MergeMultiplePipelineCacheObjects(VkDevice logical_device, VkPipelineCache target_pipeline_cache, std::vector<VkPipelineCache> const & source_pipeline_caches) {
		if (source_pipeline_caches.size() > 0) {
			VkResult result = vkMergePipelineCaches(logical_device, target_pipeline_cache, static_cast<uint32_t>(source_pipeline_caches.size()), source_pipeline_caches.data());
			if (VK_SUCCESS != result) {
				LOG( "Could not merge pipeline cache objects." );
				return false;
			}
			return true;
		}
		return false;
	}

	bool CreateGraphicsPipelines(VkDevice logical_device, std::vector<VkGraphicsPipelineCreateInfo> const & graphics_pipeline_create_infos, VkPipelineCache pipeline_cache, std::vector<VkPipeline> & graphics_pipelines) {
		if (graphics_pipeline_create_infos.size() > 0) {
			graphics_pipelines.resize(graphics_pipeline_create_infos.size());
			VkResult result = vkCreateGraphicsPipelines(logical_device, pipeline_cache, static_cast<uint32_t>(graphics_pipeline_create_infos.size()), graphics_pipeline_create_infos.data(), nullptr, graphics_pipelines.data());
			if (VK_SUCCESS != result) {
				LOG( "Could not create a graphics pipeline." );
				return false;
			}
			return true;
		}
		return false;
	}
	
	bool CreateComputePipeline(VkDevice logical_device, VkPipelineCreateFlags additional_options, VkPipelineShaderStageCreateInfo const & compute_shader_stage, VkPipelineLayout pipeline_layout, VkPipeline base_pipeline_handle, VkPipelineCache pipeline_cache, VkPipeline & compute_pipeline) {
		VkComputePipelineCreateInfo compute_pipeline_create_info = {
			VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,   // VkStructureType                    sType
			nullptr,                                          // const void                       * pNext
			additional_options,                               // VkPipelineCreateFlags              flags
			compute_shader_stage,                             // VkPipelineShaderStageCreateInfo    stage
			pipeline_layout,                                  // VkPipelineLayout                   layout
			base_pipeline_handle,                             // VkPipeline                         basePipelineHandle
			-1                                                // int32_t                            basePipelineIndex
		};

		VkResult result = vkCreateComputePipelines(logical_device, pipeline_cache, 1, &compute_pipeline_create_info, nullptr, &compute_pipeline);
		if (VK_SUCCESS != result) {
			LOG( "Could not create compute pipeline." );
			return false;
		}
		return true;
	}

	void BindPipelineObject(VkCommandBuffer command_buffer, VkPipelineBindPoint pipeline_type, VkPipeline pipeline) {
		vkCmdBindPipeline(command_buffer, pipeline_type, pipeline);
	}

	bool CreatePipelineLayoutWithCombinedImageSamplerBufferAndPushConstantRanges(VkDevice logical_device, std::vector<VkPushConstantRange> const & push_constant_ranges, VkDescriptorSetLayout & descriptor_set_layout, VkPipelineLayout & pipeline_layout) {
		std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings = {
			{
				0,                                  // uint32_t               binding
				VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,   // VkDescriptorType       descriptorType
				1,                                  // uint32_t               descriptorCount
				VK_SHADER_STAGE_FRAGMENT_BIT,       // VkShaderStageFlags     stageFlags
				nullptr                             // const VkSampler      * pImmutableSamplers
			},
			{
				1,                                  // uint32_t               binding
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,  // VkDescriptorType       descriptorType
				1,                                  // uint32_t               descriptorCount
				VK_SHADER_STAGE_VERTEX_BIT,         // VkShaderStageFlags     stageFlags
				nullptr                             // const VkSampler      * pImmutableSamplers
			}
		};

		if (!CreateDescriptorSetLayout(logical_device, descriptor_set_layout_bindings, descriptor_set_layout)) {
			return false;
		}

		if (!CreatePipelineLayout(logical_device, { descriptor_set_layout }, push_constant_ranges, pipeline_layout)) {
			return false;
		}
		return true;
	}

	bool CreateGraphicsPipelineWithVertexAndFragmentShadersDepthTestEnabledAndWithDynamicViewportAndScissorTests(VkDevice logical_device, VkPipelineCreateFlags additional_options, std::string const & vertex_shader_filename, std::string const & fragment_shader_filename, std::vector<VkVertexInputBindingDescription> const & vertex_input_binding_descriptions, std::vector<VkVertexInputAttributeDescription> const & vertex_attribute_descriptions, VkPrimitiveTopology primitive_topology, bool primitive_restart_enable, VkPolygonMode polygon_mode, VkCullModeFlags culling_mode, VkFrontFace front_face, bool logic_op_enable, VkLogicOp logic_op, std::vector<VkPipelineColorBlendAttachmentState> const & attachment_blend_states, std::array<float, 4> const & blend_constants, VkPipelineLayout pipeline_layout, VkRenderPass render_pass, uint32_t  subpass, VkPipeline base_pipeline_handle, VkPipelineCache pipeline_cache, std::vector<VkPipeline> & graphics_pipeline) {
		std::vector<unsigned char> vertex_shader_spirv;
		//if (!GetBinaryFileContents(vertex_shader_filename, vertex_shader_spirv)) {
		//	return false;
		//}
		std::vector<char> data = AST->loadFile(vertex_shader_filename.c_str());
		if (data.empty()) return false;
		vertex_shader_spirv.resize(data.size());
		memcpy(vertex_shader_spirv.data(), data.data(), data.size());

		VkDestroyer(VkShaderModule) vertex_shader_module;
		InitVkDestroyer(logical_device, vertex_shader_module);
		if (!CreateShaderModule(logical_device, vertex_shader_spirv, *vertex_shader_module)) {
			return false;
		}

		std::vector<unsigned char> fragment_shader_spirv;
		//if (!GetBinaryFileContents(fragment_shader_filename, fragment_shader_spirv)) {
		//	return false;
		//}
		data = AST->loadFile(fragment_shader_filename.c_str());
		if (data.empty()) return false;
		fragment_shader_spirv.resize(data.size());
		memcpy(fragment_shader_spirv.data(), data.data(), data.size());

		VkDestroyer(VkShaderModule) fragment_shader_module;
		InitVkDestroyer(logical_device, fragment_shader_module);
		if (!CreateShaderModule(logical_device, fragment_shader_spirv, *fragment_shader_module)) {
			return false;
		}

		std::vector<ShaderStageParameters> shader_stage_params = {
			{
				VK_SHADER_STAGE_VERTEX_BIT,   // VkShaderStageFlagBits        ShaderStage
				*vertex_shader_module,        // VkShaderModule               ShaderModule
				"main",                       // char const                 * EntryPointName
				nullptr                       // VkSpecializationInfo const * SpecializationInfo
			},
			{
				VK_SHADER_STAGE_FRAGMENT_BIT, // VkShaderStageFlagBits        ShaderStage
				*fragment_shader_module,      // VkShaderModule               ShaderModule
				"main",                       // char const                 * EntryPointName
				nullptr                       // VkSpecializationInfo const * SpecializationInfo
			}
		};

		std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos;
		SpecifyPipelineShaderStages(shader_stage_params, shader_stage_create_infos);
		
		VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info;
		SpecifyPipelineVertexInputState(vertex_input_binding_descriptions, vertex_attribute_descriptions, vertex_input_state_create_info);

		VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info;
		SpecifyPipelineInputAssemblyState(primitive_topology, primitive_restart_enable, input_assembly_state_create_info);

		ViewportInfo viewport_infos = {
			{			                // std::vector<VkViewport>   Viewports
				{
					0.0f,               // float          x
					0.0f,               // float          y
					500.0f,             // float          width
					500.0f,             // float          height
					0.0f,               // float          minDepth
					1.0f                // float          maxDepth
				}
			},
			{							// std::vector<VkRect2D>     Scissors
				{
					{                   // VkOffset2D     offset
						0,              // int32_t        x
						0               // int32_t        y
					},
					{                   // VkExtent2D     extent
						500,            // uint32_t       width
						500             // uint32_t       height
					 }
				}
			}
		};
		VkPipelineViewportStateCreateInfo viewport_state_create_info;
		SpecifyPipelineViewportAndScissorTestState(viewport_infos, viewport_state_create_info);

		VkPipelineRasterizationStateCreateInfo rasterization_state_create_info;
		SpecifyPipelineRasterizationState(false, false, polygon_mode, culling_mode, front_face, false, 0.0f, 1.0f, 0.0f, 1.0f, rasterization_state_create_info);

		VkPipelineMultisampleStateCreateInfo multisample_state_create_info;
		SpecifyPipelineMultisampleState(VK_SAMPLE_COUNT_1_BIT, false, 0.0f, nullptr, false, false, multisample_state_create_info);

		VkStencilOpState stencil_test_parameters = {
			VK_STENCIL_OP_KEEP,   // VkStencilOp    failOp
			VK_STENCIL_OP_KEEP,   // VkStencilOp    passOp
			VK_STENCIL_OP_KEEP,   // VkStencilOp    depthFailOp
			VK_COMPARE_OP_ALWAYS, // VkCompareOp    compareOp
			0,                    // uint32_t       compareMask
			0,                    // uint32_t       writeMask
			0                     // uint32_t       reference
		};
		VkPipelineDepthStencilStateCreateInfo depth_and_stencil_state_create_info;
		SpecifyPipelineDepthAndStencilState(true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0.0f, 1.0f, false, stencil_test_parameters, stencil_test_parameters, depth_and_stencil_state_create_info);

		VkPipelineColorBlendStateCreateInfo blend_state_create_info;
		SpecifyPipelineBlendState(logic_op_enable, logic_op, attachment_blend_states, blend_constants, blend_state_create_info);

		std::vector<VkDynamicState> dynamic_states = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamic_state_create_info;
		SpecifyPipelineDynamicStates(dynamic_states, dynamic_state_create_info);

		VkGraphicsPipelineCreateInfo graphics_pipeline_create_info;
		SpecifyGraphicsPipelineCreationParameters(additional_options, shader_stage_create_infos, vertex_input_state_create_info, input_assembly_state_create_info, nullptr, &viewport_state_create_info,
			rasterization_state_create_info, &multisample_state_create_info, &depth_and_stencil_state_create_info, &blend_state_create_info, &dynamic_state_create_info, pipeline_layout, render_pass,
			subpass, base_pipeline_handle, -1, graphics_pipeline_create_info);

		if (!CreateGraphicsPipelines(logical_device, { graphics_pipeline_create_info }, pipeline_cache, graphics_pipeline)) {
			return false;
		}
		return true;
	}
	     
	bool CreateMultipleGraphicsPipelinesOnMultipleThreads(VkDevice logical_device, std::string const & pipeline_cache_filename, std::vector<std::vector<VkGraphicsPipelineCreateInfo>> const & graphics_pipelines_create_infos, std::vector<std::vector<VkPipeline>> & graphics_pipelines) {
		// Load cache from file (if available)
		std::vector<unsigned char> cache_data;
		//GetBinaryFileContents(pipeline_cache_filename, cache_data);
		std::vector<char> data = AST->loadFile(pipeline_cache_filename.c_str());
		cache_data.resize( data.size() );
		memcpy(cache_data.data(), data.data(), data.size());

		// Create cache for each thread, initialize its contents with data loaded from file
		std::vector<VkDestroyer(VkPipelineCache)> pipeline_caches(graphics_pipelines_create_infos.size());
		for (size_t i = 0; i < graphics_pipelines_create_infos.size(); ++i) {
			pipeline_caches[i] = VkDestroyer(VkPipelineCache)();
			InitVkDestroyer(logical_device, pipeline_caches[i]);
			if (!CreatePipelineCacheObject(logical_device, cache_data, *pipeline_caches[i])) {
				return false;
			}
		}

		// Create multiple threads, where each thread creates multiple pipelines using its own cache
		std::vector<std::thread> threads(graphics_pipelines_create_infos.size());
		for (size_t i = 0; i < graphics_pipelines_create_infos.size(); ++i) {
			graphics_pipelines[i].resize(graphics_pipelines_create_infos[i].size());
			threads[i] = std::thread(CreateGraphicsPipelines, logical_device, std::ref(graphics_pipelines_create_infos[i]), *pipeline_caches[i], std::ref(graphics_pipelines[i]));
		}

		// Wait for all threads to finish
		for (size_t i = 0; i < graphics_pipelines_create_infos.size(); ++i) {
			threads[i].join();
		}

		// Merge all the caches into one, retrieve its contents and store them in the file
		VkPipelineCache target_cache = *pipeline_caches.back();
		std::vector<VkPipelineCache> source_caches(pipeline_caches.size() - 1);
		for (size_t i = 0; i < pipeline_caches.size() - 1; ++i) {
			source_caches[i] = *pipeline_caches[i];
		}

		if (!MergeMultiplePipelineCacheObjects(logical_device, target_cache, source_caches)) {
			return false;
		}

		if (!RetrieveDataFromPipelineCache(logical_device, target_cache, cache_data)) {
			return false;
		}

		//if( !SaveBinaryFile( pipeline_cache_filename, cache_data ) ) {
		//  return false;
		//}

		return true;
	}

	void DestroyPipeline(VkDevice logical_device, VkPipeline & pipeline) {
		if (VK_NULL_HANDLE != pipeline) {
			vkDestroyPipeline(logical_device, pipeline, nullptr);
			pipeline = VK_NULL_HANDLE;
		}
	}

	void DestroyPipelineCache(VkDevice logical_device, VkPipelineCache & pipeline_cache) {
		if (VK_NULL_HANDLE != pipeline_cache) {
			vkDestroyPipelineCache(logical_device, pipeline_cache, nullptr);
			pipeline_cache = VK_NULL_HANDLE;
		}
	}

	void DestroyPipelineLayout(VkDevice logical_device, VkPipelineLayout & pipeline_layout) {
		if (VK_NULL_HANDLE != pipeline_layout) {
			vkDestroyPipelineLayout(logical_device, pipeline_layout, nullptr);
			pipeline_layout = VK_NULL_HANDLE;
		}
	}

	void DestroyShaderModule(VkDevice logical_device, VkShaderModule & shader_module) {
		if (VK_NULL_HANDLE != shader_module) {
			vkDestroyShaderModule(logical_device, shader_module, nullptr);
			shader_module = VK_NULL_HANDLE;
		}
	}

	void ClearColorImage(VkCommandBuffer command_buffer, VkImage image, VkImageLayout image_layout, std::vector<VkImageSubresourceRange> const & image_subresource_ranges, VkClearColorValue & clear_color) {
		vkCmdClearColorImage(command_buffer, image, image_layout, &clear_color, static_cast<uint32_t>(image_subresource_ranges.size()), image_subresource_ranges.data());
	}

	void ClearDepthStencilImage(VkCommandBuffer command_buffer, VkImage image, VkImageLayout image_layout, std::vector<VkImageSubresourceRange> const & image_subresource_ranges, VkClearDepthStencilValue & clear_value) {
		vkCmdClearDepthStencilImage(command_buffer, image, image_layout, &clear_value, static_cast<uint32_t>(image_subresource_ranges.size()), image_subresource_ranges.data());
	}

	void ClearRenderPassAttachments(VkCommandBuffer command_buffer, std::vector<VkClearAttachment> const & attachments, std::vector<VkClearRect> const & rects) {
		vkCmdClearAttachments(command_buffer, static_cast<uint32_t>(attachments.size()), attachments.data(), static_cast<uint32_t>(rects.size()), rects.data());
	}

	void BindVertexBuffers(VkCommandBuffer command_buffer, uint32_t first_binding, std::vector<VertexBufferParameters> const & buffers_parameters) {
		if (buffers_parameters.size() > 0) {
			std::vector<VkBuffer>     buffers;
			std::vector<VkDeviceSize> offsets;
			for (auto & buffer_parameters : buffers_parameters) {
				buffers.push_back(buffer_parameters.Buffer);
				offsets.push_back(buffer_parameters.MemoryOffset);
			}
			vkCmdBindVertexBuffers(command_buffer, first_binding, static_cast<uint32_t>(buffers_parameters.size()), buffers.data(), offsets.data());
		}
	}

	void BindIndexBuffer(VkCommandBuffer command_buffer, VkBuffer buffer, VkDeviceSize memory_offset, VkIndexType index_type) {
		vkCmdBindIndexBuffer(command_buffer, buffer, memory_offset, index_type);
	}

	void ProvideDataToShadersThroughPushConstants(VkCommandBuffer command_buffer, VkPipelineLayout pipeline_layout, VkShaderStageFlags pipeline_stages, uint32_t offset, uint32_t size, void * data) {
		vkCmdPushConstants(command_buffer, pipeline_layout, pipeline_stages, offset, size, data);
	}

	void SetViewportStateDynamically(VkCommandBuffer command_buffer, uint32_t first_viewport, std::vector<VkViewport> const & viewports) {
		vkCmdSetViewport(command_buffer, first_viewport, static_cast<uint32_t>(viewports.size()), viewports.data());
	}

	void SetScissorStateDynamically(VkCommandBuffer command_buffer, uint32_t first_scissor, std::vector<VkRect2D> const & scissors) {
		vkCmdSetScissor(command_buffer, first_scissor, static_cast<uint32_t>(scissors.size()), scissors.data());
	}

	void SetLineWidthStateDynamically(VkCommandBuffer command_buffer, float line_width) {
		vkCmdSetLineWidth(command_buffer, line_width);
	}

	void SetDepthBiasStateDynamically(VkCommandBuffer command_buffer, float constant_factor, float clamp, float slope_factor) {
		vkCmdSetDepthBias(command_buffer, constant_factor, clamp, slope_factor);
	}

	void SetBlendConstantsStateDynamically(VkCommandBuffer command_buffer, std::array<float, 4> const & blend_constants) {
		vkCmdSetBlendConstants(command_buffer, blend_constants.data());
	}

	void DrawGeometry(VkCommandBuffer command_buffer, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) {
		vkCmdDraw(command_buffer, vertex_count, instance_count, first_vertex, first_instance);
	}

	void DrawIndexedGeometry(VkCommandBuffer command_buffer, uint32_t index_count, uint32_t instance_count, uint32_t first_index, uint32_t vertex_offset, uint32_t first_instance) {
		vkCmdDrawIndexed(command_buffer, index_count, instance_count, first_index, vertex_offset, first_instance);
	}

	void DispatchComputeWork(VkCommandBuffer command_buffer, uint32_t x_size, uint32_t y_size, uint32_t z_size) {
		vkCmdDispatch(command_buffer, x_size, y_size, z_size);
	}

	void ExecuteSecondaryCommandBufferInsidePrimaryCommandBuffer(VkCommandBuffer command_buffer, std::vector<VkCommandBuffer> const & secondary_command_buffers) {
		if (secondary_command_buffers.size() > 0) {
			vkCmdExecuteCommands(command_buffer, static_cast<uint32_t>(secondary_command_buffers.size()), secondary_command_buffers.data());
		}
	}

	bool RecordCommandBufferThatDrawsGeometryWithDynamicViewportAndScissorStates(VkCommandBuffer command_buffer, VkImage swapchain_image, uint32_t present_queue_family_index, uint32_t graphics_queue_family_index, VkRenderPass render_pass, VkFramebuffer framebuffer, VkExtent2D framebuffer_size, std::vector<VkClearValue> const & clear_values, VkPipeline graphics_pipeline, uint32_t first_vertex_buffer_binding, std::vector<VertexBufferParameters> const & vertex_buffers_parameters, VkPipelineLayout pipeline_layout, std::vector<VkDescriptorSet> const & descriptor_sets, uint32_t index_for_first_descriptor_set, Mesh const & geometry, uint32_t instance_count, uint32_t first_instance) {
		if (!BeginCommandBufferRecordingOperation(command_buffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr)) {
			return false;
		}

		if (present_queue_family_index != graphics_queue_family_index) {
			ImageTransition image_transition_before_drawing = {
				swapchain_image,                          // VkImage              Image
				VK_ACCESS_MEMORY_READ_BIT,                // VkAccessFlags        CurrentAccess
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,     // VkAccessFlags        NewAccess
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,          // VkImageLayout        CurrentLayout
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,          // VkImageLayout        NewLayout
				present_queue_family_index,               // uint32_t             CurrentQueueFamily
				graphics_queue_family_index,              // uint32_t             NewQueueFamily
				VK_IMAGE_ASPECT_COLOR_BIT                 // VkImageAspectFlags   Aspect
			};
			SetImageMemoryBarrier(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, { image_transition_before_drawing });
		}

		BeginRenderPass(command_buffer, render_pass, framebuffer, { { 0, 0 }, framebuffer_size }, clear_values, VK_SUBPASS_CONTENTS_INLINE);

		BindPipelineObject(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);

		VkViewport viewport = {
			0.0f,                                         // float          x
			0.0f,                                         // float          y
			static_cast<float>(framebuffer_size.width),   // float          width
			static_cast<float>(framebuffer_size.height),  // float          height
			0.0f,                                         // float          minDepth
			1.0f,                                         // float          maxDepth
		};
		SetViewportStateDynamically(command_buffer, 0, { viewport });

		VkRect2D scissor = {
			{                                             // VkOffset2D     offset
				0,                                        // int32_t        x
				0                                         // int32_t        y
			},
			{                                             // VkExtent2D     extent
				framebuffer_size.width,                   // uint32_t       width
				framebuffer_size.height                   // uint32_t       height
			}
		};

		SetScissorStateDynamically(command_buffer, 0, { scissor });

		BindVertexBuffers(command_buffer, first_vertex_buffer_binding, vertex_buffers_parameters);

		if (descriptor_sets.size() > 0) {
			BindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, index_for_first_descriptor_set, descriptor_sets, {});
		}
		for (size_t i = 0; i < geometry.Parts.size(); ++i) {
			DrawGeometry(command_buffer, geometry.Parts[i].VertexCount, instance_count, geometry.Parts[i].VertexOffset, first_instance);
		}

		EndRenderPass(command_buffer);

		if (present_queue_family_index != graphics_queue_family_index) {
			ImageTransition image_transition_before_present = {
				swapchain_image,                          // VkImage              Image
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,     // VkAccessFlags        CurrentAccess
				VK_ACCESS_MEMORY_READ_BIT,                // VkAccessFlags        NewAccess
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,          // VkImageLayout        CurrentLayout
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,          // VkImageLayout        NewLayout
				graphics_queue_family_index,              // uint32_t             CurrentQueueFamily
				present_queue_family_index,               // uint32_t             NewQueueFamily
				VK_IMAGE_ASPECT_COLOR_BIT                 // VkImageAspectFlags   Aspect
			};
			SetImageMemoryBarrier(command_buffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, { image_transition_before_present });
		}

		if (!EndCommandBufferRecordingOperation(command_buffer)) {
			return false;
		}
		return true;
	}

	bool RecordCommandBuffersOnMultipleThreads(std::vector<CommandBufferRecordingThreadParameters> const & threads_parameters, VkQueue queue, std::vector<WaitSemaphoreInfo> wait_semaphore_infos, std::vector<VkSemaphore> signal_semaphores, VkFence fence) {
		std::vector<std::thread> threads(threads_parameters.size());
		for (size_t i = 0; i < threads_parameters.size(); ++i) {
			threads[i] = std::thread(threads_parameters[i].RecordingFunction, threads_parameters[i].CommandBuffer);
		}

		std::vector<VkCommandBuffer> command_buffers(threads_parameters.size());
		for (size_t i = 0; i < threads_parameters.size(); ++i) {
			threads[i].join();
			command_buffers[i] = threads_parameters[i].CommandBuffer;
		}

		if (!SubmitCommandBuffersToQueue(queue, wait_semaphore_infos, command_buffers, signal_semaphores, fence)) {
			return false;
		}
		return true;
	}

	bool PrepareSingleFrameOfAnimation(VkDevice logical_device, VkQueue graphics_queue, VkQueue present_queue, VkSwapchainKHR swapchain, VkExtent2D swapchain_size, std::vector<VkImageView> const & swapchain_image_views, VkImageView depth_attachment, std::vector<WaitSemaphoreInfo> const & wait_infos, VkSemaphore image_acquired_semaphore, VkSemaphore ready_to_present_semaphore, VkFence finished_drawing_fence, std::function<bool(VkCommandBuffer, uint32_t, VkFramebuffer)> record_command_buffer, VkCommandBuffer command_buffer, VkRenderPass render_pass, VkDestroyer(VkFramebuffer) & framebuffer) {
		uint32_t image_index;
		if (!AcquireSwapchainImage(logical_device, swapchain, image_acquired_semaphore, VK_NULL_HANDLE, image_index)) {
			return false;
		}

		std::vector<VkImageView> attachments = { swapchain_image_views[image_index] };
		if (VK_NULL_HANDLE != depth_attachment) {
			attachments.push_back(depth_attachment);
		}
		if (!CreateFramebuffer(logical_device, render_pass, attachments, swapchain_size.width, swapchain_size.height, 1, *framebuffer)) {
			return false;
		}

		if (!record_command_buffer(command_buffer, image_index, *framebuffer)) {
			return false;
		}

		std::vector<WaitSemaphoreInfo> wait_semaphore_infos = wait_infos;
		wait_semaphore_infos.push_back({
			image_acquired_semaphore,                     // VkSemaphore            Semaphore
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT // VkPipelineStageFlags   WaitingStage
			});
		if (!SubmitCommandBuffersToQueue(graphics_queue, wait_semaphore_infos, { command_buffer }, { ready_to_present_semaphore }, finished_drawing_fence)) {
			return false;
		}

		PresentInfo present_info = {
			swapchain,                                    // VkSwapchainKHR         Swapchain
			image_index                                   // uint32_t               ImageIndex
		};
		if (!PresentImage(present_queue, { ready_to_present_semaphore }, { present_info })) {
			return false;
		}
		return true;
	}

	bool IncreasePerformanceThroughIncreasingTheNumberOfSeparatelyRenderedFrames(VkDevice logical_device, VkQueue graphics_queue, VkQueue present_queue, VkSwapchainKHR swapchain, VkExtent2D swapchain_size, std::vector<VkImageView> const & swapchain_image_views, VkRenderPass render_pass, std::vector<WaitSemaphoreInfo> const & wait_infos, std::function<bool(VkCommandBuffer, uint32_t, VkFramebuffer)> record_command_buffer, std::vector<FrameResources> & frame_resources) {
		static uint32_t frame_index = 0;
		FrameResources & current_frame = frame_resources[frame_index];

		if (!WaitForFences(logical_device, { *current_frame.DrawingFinishedFence }, false, 2000000000)) {
			return false;
		}
		if (!ResetFences(logical_device, { *current_frame.DrawingFinishedFence })) {
			return false;
		}

		InitVkDestroyer(logical_device, current_frame.Framebuffer);

		if (!PrepareSingleFrameOfAnimation(logical_device, graphics_queue, present_queue, swapchain, swapchain_size, swapchain_image_views,
			*current_frame.DepthAttachment, wait_infos, *current_frame.ImageAcquiredSemaphore, *current_frame.ReadyToPresentSemaphore,
			*current_frame.DrawingFinishedFence, record_command_buffer, current_frame.CommandBuffer, render_pass, current_frame.Framebuffer)) {
			return false;
		}

		frame_index = (frame_index + 1) % frame_resources.size();
		return true;
	}

} // namespace VulkanCookbook
*/
#pragma once

#include "header.h"

struct QeDataEnvironment {
    QeDataCamera camera;
    AeArray<float,4> param;  // 0: gamma, 1: exposure
};

struct QeDataViewport {
    VkViewport viewport;
    VkRect2D scissor;
    QeCamera *camera = nullptr;
    // std::vector<QeLight*> lights;

    QeDataDescriptorSet descriptorSetComputeRayTracing;
    QeDataComputePipeline computePipelineRayTracing;

    QeDataDescriptorSet commonDescriptorSet;
    QeDataEnvironment environmentData;
    QeVKBuffer environmentBuffer;

    QeDataViewport()
        : environmentBuffer(eBuffer_uniform),
          commonDescriptorSet(eDescriptorSetLayout_Common),
          descriptorSetComputeRayTracing(eDescriptorSetLayout_Raytracing) {}

    //~QeDataViewport();
};

struct QeBufferSubpass {
    AeArray<float, 4> param;  // 0: blurHorizontal, 1: blurScale, 2: blurStrength
};

struct QeDataSubpass {
    QeBufferSubpass bufferData;
    QeVKBuffer buffer;
    QeAssetGraphicsShader graphicsShader;
    QeDataDescriptorSet descriptorSet;
    QeDataGraphicsPipeline graphicsPipeline;

    QeDataSubpass() : buffer(eBuffer_uniform), descriptorSet(eDescriptorSetLayout_Postprocessing) {}
    //~QeDataSubpass();
};

struct QeDataRender {
    std::vector<QeDataViewport *> viewports;
    int cameraOID;
    VkViewport viewport;
    VkRect2D scissor;

    QeVKImage colorImage, colorImage2, depthStencilImage,
        multiSampleColorImage;  // , multiSampleDepthStencilImage;
    std::vector<VkFramebuffer> frameBuffers;

    std::vector<VkCommandBuffer> commandBuffers;
    VkSemaphore semaphore = VK_NULL_HANDLE;

    VkRenderPass renderPass = VK_NULL_HANDLE;
    std::vector<QeDataSubpass *> subpass;

    QeDataRender()
        : colorImage(eImage_render),
          colorImage2(eImage_render),
          depthStencilImage(eImage_depthStencil),
          multiSampleColorImage(eImage_attach) /*,
                                                  multiSampleDepthStencilImage(eImage_multiSampleDepthStencil)*/
    {}
    ~QeDataRender();
    void clear();
};

struct QeDataSwapchain {
    VkSwapchainKHR khr = VK_NULL_HANDLE;
    VkExtent2D extent;
    VkFormat format;
    std::vector<QeVKImage> images;
};

struct QeDataDrawCommand {
    VkCommandBuffer commandBuffer;
    QeCamera *camera;
    QeDataDescriptorSet *commonDescriptorSet;
    VkRenderPass renderPass;
    AE_RENDER_TYPE type;
};

class QeGraphics {
   public:
    QeGraphics(AeGlobalKey &_key) : lightsBuffer(eBuffer_storage), modelDatasBuffer(eBuffer_storage) {}
    ~QeGraphics();

    std::vector<QeDataRender *> renders;
    QeDataSwapchain swapchain;
    QeRenderSetting *renderSetting = nullptr;

    // int currentTargetViewport = 0;
    QeCamera *currentCamera = nullptr;
    VkSemaphore renderCompleteSemaphore = VK_NULL_HANDLE;
    std::vector<VkFence> fences;

    std::vector<QeModel *> models;
    std::vector<QeModel *> alphaModels;
    std::vector<QeModel *> models2D;
    std::vector<QeLight *> lights;
    QeVKBuffer modelDatasBuffer;
    QeVKBuffer lightsBuffer;
    bool bUpdateLight = false;

    void initialize();
    void clear();
    bool addPostProcssing(AE_RENDER_TYPE renderType, int cameraOID, int postprocessingOID);
    void addNewViewport(AE_RENDER_TYPE type);
    void popViewport(AE_RENDER_TYPE type);
    void updateViewport();
    void updateBuffer();
    void add2DModel(QeModel *model);
    void addLight(QeLight *light);
    void removeLight(QeLight *light);
    void update1();
    void update2();
    void setTargetCamera(int cameraOID);
    QeCamera *getTargetCamera();
    QeDataRender *getRender(AE_RENDER_TYPE type, int cameraOID);
    // bool bUpdateComputeCommandBuffers = false;
    // bool bUpdateDrawCommandBuffers = false;
    bool bRecreateRender = false;

    // std::vector<VkSemaphore> computeSemaphores;
    // std::vector<VkFence> computeFences;

    // VkSemaphore textOverlayComplete;

    QeDataRender *createRender(AE_RENDER_TYPE type, int cameraOID, AeArray<int, 2> renderSize);
    void refreshRender();
    void cleanupRender();
    void drawFrame();
    void updateDrawCommandBuffers();
    // void updateComputeCommandBuffers();

    void sortAlphaModels(QeCamera *camera);

    void updateComputeCommandBuffer(VkCommandBuffer &commandBuffer);
    void updateDrawCommandBuffer(QeDataDrawCommand *command);
};

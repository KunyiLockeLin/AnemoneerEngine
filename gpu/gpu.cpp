#include "gpu.h"
#include "vulkan_objects.h"
#include<assert.h>

BEGIN_NAMESPACE(ae)
BEGIN_NAMESPACE(gpu)

/*
Interface table:
IDevice     <-> object::Device
IRendering  <-> object::Rendering
*/
template <typename T, typename K>
T* convert_interface(K* object ) {
    return reinterpret_cast<T*>(object);
}

AeResult initialize(const InitializeInfo& initialize_info, IDevice*& device) {
    ASSERT_SUCCESS(vk::MGR.initialize());

    std::shared_ptr<vk::Device> obj_device;
    ASSERT_SUCCESS(vk::MGR.get<vk::Device>(obj_device));
    ASSERT_SUCCESS(obj_device->initialize(initialize_info));
    device = convert_interface<IDevice>(obj_device.get());
    return AE_SUCCESS;
}

AeResult cleanup() {
    ASSERT_SUCCESS(vk::MGR.cleanup());
    return AE_SUCCESS;
}

AeResult pre_update() {
    ASSERT_SUCCESS(vk::MGR.pre_update());
    return AE_SUCCESS;
}

AeResult update() {
    ASSERT_SUCCESS(vk::MGR.update());
    return AE_SUCCESS;
}

AeResult post_update() {
    ASSERT_SUCCESS(vk::MGR.post_update());
    return AE_SUCCESS;
}

IRendering* IDevice::get_rendering() {
    std::shared_ptr<vk::Rendering> obj_rendering;
    ASSERT_SUCCESS(vk::MGR.get<vk::Rendering>(obj_rendering));
    return convert_interface<IRendering>(obj_rendering.get());
}

END_NAMESPACE(gpu)
END_NAMESPACE(ae)

# Vulkan

  - enviroment: Visual Studio 2017 and VulkanSDK 1.0.65.1
  - Setting: data/config.ini
  - Input:
    - "+" : Add new viewport. The maximum number of viewport is nine.
    - "-" : Substract the last viewport.
    - "1"~"9" : Choose a viewport to control its camera.
    - "R" : Reset the camera.
    - "Up","Down","Left","Right","W","A","S","D","Q","E","MouseLeftMove","MouseRightMove" : Move the camera.
    
# ToDoList:
  - 1. Memory Management and Object Management are weak.
  - 2. It only can run in debug mode. It crashes in the others. 
  - 3. I think the light calculation is not correct in fragment shader.
  - 4. Replace .obj and .mlt files to glTF for bone animation.
  - 5. Load .jpg and .png image, instead of .bmp32.
  - 6. Attach obj.
  - 7. Bone animation.
  - 8. Normal map and specular map.
  - 9. Combine socket https://github.com/KunyiLockeLin/WebSocket_server
  - 10. Separate different libraries.
  - 11. bunding Box
  - 12. Frustum Culling
  - 13. Room
  - 14. Timer
 
# Reference:
  - https://www.khronos.org/blog/beginners-guide-to-vulkan
  - https://renderdoc.org/vulkan-in-30-minutes.html
  - https://vulkan-tutorial.com/
  - https://github.com/KhronosGroup/Khronosdotorg/blob/master/api/vulkan/resources.md
  - https://github.com/LunarG/VulkanSamples
  - https://github.com/SaschaWillems/Vulkan

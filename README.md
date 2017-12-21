# Vulkan

  - enviroment: Visual Studio 2017 and VulkanSDK 1.0.65.1
  - Setting: data/config.ini
  - Input:
    - "+" : Add new viewport. The maximum number of viewport is nine.
    - "-" : Substract the last viewport.
    - "1"~"9" : Choose a viewport to control its camera.
    - "R" : Reset the camera.
    - "Up","Down","Left","Right","W","A","S","D","Q","E","MouseLeftMove","MouseRightMove" : Move the camera.
    
    ![alt text](https://github.com/KunyiLockeLin/Vulkan/blob/master/sample01.jpg)
    
# ToDoList:
  - 1. It only can run in debug mode. It crashes in the others. 
  - 2. Memory Management and Object Management are weak.
  - 3. I think the light calculation is not correct in fragment shader.
  - 4. Replace .obj and .mlt files to .glTF for bone animation.
  - 5. Load .jpg and .png image, instead of .bmp 32bits.
  - 6. Attach obj.
  - 7. Bone animation.
  - 8. Normal map and specular map.
  - 9. Combine socket https://github.com/KunyiLockeLin/WebSocket_server
  - 10. Separate different libraries.
  - 11. bounding Box
  - 12. Collision
  - 13. Frustum Culling
  - 14. Room
  - 15. Timer
  - 16. HDR
 
# Reference:
  - https://www.khronos.org/blog/beginners-guide-to-vulkan
  - https://renderdoc.org/vulkan-in-30-minutes.html
  - https://vulkan-tutorial.com/
  - https://github.com/KhronosGroup/Khronosdotorg/blob/master/api/vulkan/resources.md
  - https://github.com/LunarG/VulkanSamples
  - https://github.com/SaschaWillems/Vulkan
  - https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
  - https://github.com/KhronosGroup/glTF-Tutorials/tree/master/gltfTutorial

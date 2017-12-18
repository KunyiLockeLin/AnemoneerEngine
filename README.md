# Vulkan

enviroment: Visual Studio 2017 and VulkanSDK 1.0.65.1


Setting: data/config.ini


Input:

  "+" : Add new viewport. The maximum number of viewport is nine.

  "-" : Substract the last viewport.

  "1"~"9" : Choose a viewport to control its camera.

  "R" : Reset the camera.

  "Up","Down","Left","Right","W","A","S","D","Q","E","MouseLeftMove","MouseRightMove" : Move the camera.


future:
  1. Memory Management and Object Management are weak.
  
  2. I think the light calculation is not correct in fragment shader.
  
  3. Replace .obj and .mlt files to glTF for bone animation.
  
  4. Attach obj.
  
  5. Bone animation.
  
  6. Normal map and specular map.
  
  7. Combine socket https://github.com/KunyiLockeLin/WebSocket_server
  
  8. Separate different libraries.

  9. Bounding Box

 10. Frustum Culling
 
 11. Room
  
 12. Timer
 
Reference:

https://www.khronos.org/blog/beginners-guide-to-vulkan &

https://renderdoc.org/vulkan-in-30-minutes.html &

https://vulkan-tutorial.com/ &

https://github.com/KhronosGroup/Khronosdotorg/blob/master/api/vulkan/resources.md &

https://github.com/LunarG/VulkanSamples &

https://github.com/SaschaWillems/Vulkan

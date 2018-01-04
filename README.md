# Vulkan

  - enviroment: Visual Studio 2017 and VulkanSDK 1.0.65.1
  - Setting: data/config.xml
  - Input:
    - "+" : Add new viewport. The maximum number of viewport is nine.
    - "-" : Substract the last viewport.
    - "1"~"9" : Choose a viewport to control its camera.
    - "R" : Reset the camera.
    - "Up","Down","Left","Right","W","A","S","D","Q","E","MouseLeftMove","MouseRightMove" : Move the camera.
    
    ![alt text](https://github.com/KunyiLockeLin/Vulkan/blob/master/sample01.jpg)
    
# Feature:
  - Comptuer Graphics's power is by Vulkan. VulkanSDK is only the third party llibrary.
  - Support mutli-viewports.
  - Support simple light and camera control.
  - Run vertex, fragment, and geometry shader.
  - For model  - decode obj, mtl.
  - For image  - decode png, jpeg, bmp.
  - For string - decode xml, json.
  - Decode deflate, Huffman code.
  
# ToDoList:
  - Replace .obj and .mlt files to .glTF for bone animation.
  - It only can run in debug mode. It crashes in the others. 
  - Memory Management and Object Management are weak.
  - I think the light calculation is not correct in fragment shader.
  - Attach obj.
  - Bone animation.
  - Normal map and specular map.
  - Combine socket https://github.com/KunyiLockeLin/WebSocket_server
  - Separate different libraries.
  - bounding Box
  - Collision
  - Frustum Culling
  - Room
  - Timer
  - HDR
 
# Reference:
  - https://www.khronos.org/blog/beginners-guide-to-vulkan
  - https://renderdoc.org/vulkan-in-30-minutes.html
  - https://vulkan-tutorial.com/
  - https://github.com/KhronosGroup/Khronosdotorg/blob/master/api/vulkan/resources.md
  - https://github.com/LunarG/VulkanSamples
  - https://github.com/SaschaWillems/Vulkan
  - https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
  - https://github.com/KhronosGroup/glTF-Tutorials/tree/master/gltfTutorial
  - http://lodev.org/lodepng/

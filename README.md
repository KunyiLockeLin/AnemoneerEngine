# Vulkan

  - enviroment: Visual Studio 2017 and VulkanSDK 1.0.65.1
  
    ![alt text](https://github.com/KunyiLockeLin/Vulkan/blob/master/sample01.jpg)
    
# Feature:
  - Comptuer Graphics's power is by Vulkan. VulkanSDK is a only third party llibrary. The window and UI are from WinAPI.
  - Support mutli-viewports, post-processing.
  - Support simple light and camera control.
  - Support vertex, geometry, and fragment shader.
  - Support Phong shading and PBR(Physically-Based Rendering)
  - Support diffuse/baseColor map.
  - Support skeletal animation(Only for gltf), attach obj and billboards.
  - Support Log output, and command.
  - For model  - obj, mtl, gltf.
  - For image  - png, jpeg, bmp.
  - For string - xml, json.
  - Decode deflate, Huffman code, png, jpeg.
  - Math library.
  - Setting: data/config.xml
  - Input:
    - "+" : Add new viewport. The maximum number of viewport is nine.
    - "-" : Substract the last viewport.
    - "1"~"9" : Choose a viewport to control its camera.
    - "Up","Down","Left","Right","W","A","S","D","Q","E","MouseLeftMove","MouseRightMove" : Move the camera.
    - "c" : turn on command mode.
      - "resetcamera [p1]" : Initialize the camera. p1 is camera mode. 0 is third person. 1 is first person. If p1 is empty, it means not to change the camera mode.
      - "restart [p1]" : Restart the programe. p1 is scene name. If p1 is empty, it means to restart the same scene. In config.xml, it has scene1, scene2, and scene3.
      
# ToDoList:
  - PBR lighting isn't smooth.
  - multiple render
  - HDR
  - mirror
  - bounding Box
  - collision
  - Terrian system
  - Dynamic Decals
  - normal map, specular map, cube map, image based lighting map, metallicRoughness map, occlusion map, emissive map.
  - shadow
  - mix animation
  - frustum Culling
  - room
  - combine socket https://github.com/KunyiLockeLin/WebSocket_server
  - separate to many different libraries
  - release mode crash.
  - camera control modification
  
# Reference:
  - https://www.khronos.org/blog/beginners-guide-to-vulkan
  - https://renderdoc.org/vulkan-in-30-minutes.html
  - https://vulkan-tutorial.com/
  - https://github.com/PacktPublishing/Vulkan-Cookbook
  - https://github.com/KhronosGroup/Khronosdotorg/blob/master/api/vulkan/resources.md
  - https://github.com/SaschaWillems/Vulkan
  - https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
  - https://github.com/KhronosGroup/glTF-Tutorials/tree/master/gltfTutorial
  - https://github.com/KhronosGroup/glTF-Blender-Exporter
  - http://lodev.org/lodepng/
  - https://github.com/KhronosGroup/glTF-WebGL-PBR
  - https://github.com/TheThinMatrix/OpenGL-Animation
  - https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt
  - https://learnopengl.com/#!Advanced-OpenGL/Advanced-GLSL
  - https://github.com/KhronosGroup/Vulkan-Docs/issues/274

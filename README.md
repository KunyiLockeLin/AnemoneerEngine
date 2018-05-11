# Vulkan
  - enviroment: Visual Studio 2017 and VulkanSDK 1.0.73.0
  
    ![alt text](https://github.com/KunyiLockeLin/Vulkan/blob/master/sample01.jpg)
    
# Feature:
  - Comptuer Graphics's power is by Vulkan. VulkanSDK is a only third party llibrary. The window and UI are from WinAPI.
  - Support mutli-viewports, post-processing.
  - Support simple light and camera control.
  - Support vertex, geometry, and fragment shader.
  - Support ~~Phong shading~~ and PBR(Physically-Based Rendering)
  - Support diffuse/baseColor map.
  - Support skeletal animation(Only for gltf), attach obj and billboards.
  - Support Log output, and command.
  - For model  - ~~obj, mtl~~, gltf.
  - For image  - png, jpeg, bmp.
  - For string - xml, json.
  - Decode deflate, Huffman code, png, jpeg.
  - Math library.
  - Setting: data/config.xml
  - Input:
    - "esc" : Close the program.
    - "+" : Add a new viewport. The maximum number of viewport is nine.
    - "-" : Remove the last viewport.
    - "1"~"9" : Choose a viewport to control its camera.
    - "Up","Down","Left","Right","W","A","S","D","Q","E","MouseLeftMove","MouseRightMove" : Move the camera.
    - "/" : turn on command mode or input directly in console.
      - "resetcamera [p1]" : Initialize the camera. p1 is camera mode. 0 is third person. 1 is first person. If p1 is empty, it means not to change the camera mode.
      - "restart [p1]" : Restart the programe. p1 is scene name. If p1 is empty, it means to restart the same scene. In config.xml, it has scene1, scene2, and scene3.

# config.xml:

# ToDoList:
  - PBR lighting isn't smooth.
  - multiple render
  - mirror
  - study 
    - https://github.com/PacktPublishing/Vulkan-Cookbook 
    - https://github.com/SaschaWillems/Vulkan 
    - https://renderdoc.org/docs/index.html
    - https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf
---
  - ui
  - bounding Box and sphere
  - collision
  - Terrain system
  - parallel programming for collision and physical
  - physical
  - select object
  - convert the whole assets into a single binary data
---
  - combine socket https://github.com/KunyiLockeLin/WebSocket_server
  - release mode crash
  - L-system, Swarm grammar
  - camera control modification
  - HDR
  - frustum Culling
  - Dynamic Decals
  - normal map, specular map, cube map, image based lighting map, metallicRoughness map, occlusion map, emissive map.
  - shadow
  - mix animation
  - room
  - C-style
  - combine browser
  - live stream
  - machine learning for performance
  
# Reference:
  - https://vulkan-tutorial.com/
  - https://github.com/PacktPublishing/Vulkan-Cookbook
  - https://github.com/SaschaWillems/Vulkan
  - https://github.com/pandurangkatkar/Vulkan-1
  - https://www.khronos.org/blog/beginners-guide-to-vulkan
  - https://renderdoc.org/vulkan-in-30-minutes.html
  - https://github.com/KhronosGroup/Khronosdotorg/blob/master/api/vulkan/resources.md
  - https://github.com/LunarG/VulkanSamples
  - https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
  - https://github.com/KhronosGroup/glTF-Tutorials/tree/master/gltfTutorial
  - https://github.com/KhronosGroup/glTF-Blender-Exporter
  - http://lodev.org/lodepng/
  - https://github.com/KhronosGroup/glTF-WebGL-PBR
  - https://github.com/TheThinMatrix/OpenGL-Animation
  - https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt
  - https://learnopengl.com/#!Advanced-OpenGL/Advanced-GLSL
  - https://github.com/KhronosGroup/Vulkan-Docs/issues/274
  - https://github.com/chunying/gaminganywhere
  - https://gpuopen.com/
  - https://youtu.be/PxopE6Lwz3g

  - https://github.com/LunarG/VulkanTools/blob/master/layersvt/screenshot.cpp
  - https://github.com/LunarG/VulkanTools/blob/master/layersvt/screenshot_parsing.cpp
  - https://forums.khronos.org/showthread.php/13015-How-vkCreateSwapchainKHR-from-Image-Not-from-surface
  - https://stackoverflow.com/questions/38885309/is-it-possible-to-do-offscreen-rendering-without-surface-in-vulkan/38885394
  - https://github.com/SaschaWillems/Vulkan/blob/master/examples/offscreen/offscreen.cpp
  - https://www.khronos.org/registry/vulkan/specs/1.0/man/html/vkCmdUpdateBuffer.html

  - live stream websocket, Htc pro vive, Peregrine RPC, Bytestream, select(), Use UDP and check date correctly by myself
  - https://www.youtube.com/watch?v=WcwnQW_AnC8
  - https://play.google.com/store/apps/details?id=jp.co.sqex.game.ff13

  - https://github.com/distcc/distcc
  - https://en.wikipedia.org/wiki/Distcc
  - https://en.wikipedia.org/wiki/OpenMP

  - PolyWorld https://en.wikipedia.org/wiki/Polyworld
  - Lindenmayer https://en.wikipedia.org/wiki/L-system
  - http://www.kevs3d.co.uk/dev/lsystems/
  - https://youtu.be/PxopE6Lwz3g

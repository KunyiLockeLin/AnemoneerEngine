# QueenEngine
  - enviroment: Visual Studio 2017, VulkanSDK 1.1.82.1
  
  - scene 28000 - mulit-render, outline(stencil buffer)
  ![alt text](https://github.com/KunyiLockeLin/QueenEngine_Vulkan/blob/master/scene28000.gif)
  
  - scene 28001 - cubemap, skeletal animation, attach obj
  ![alt text](https://github.com/KunyiLockeLin/QueenEngine_Vulkan/blob/master/scene28001.gif)
    
  - scene 28002 - paritcle(compute shader)
  ![alt text](https://github.com/KunyiLockeLin/QueenEngine_Vulkan/blob/master/scene28002.gif)
    
  - scene 28003 - post-processing(grayscale), show noraml & mesh, change scene
  ![alt text](https://github.com/KunyiLockeLin/QueenEngine_Vulkan/blob/master/scene28003.gif)
    
  - mutli-viewports                   
  ![alt text](https://github.com/KunyiLockeLin/QueenEngine_Vulkan/blob/master/viewport.gif)
    
# Feature:
  - Comptuer Graphics's power is by Vulkan API. The window and UI are from WinAPI.
  - Support mutli-viewports, post-processing.
  - Support light and camera control.
  - Support vertex, tessellation control & evaluation(triangle-faces become opposite(cw)), geometry, fragment shader, compute shader(particles).
  - Support ~~Phong shading~~ and PBR(Physically-Based Rendering).     
  - Support diffuse/baseColor map, cubemap & reflection & refraction, normal map.
  - Support skeletal animation, attach obj, draw lines, billboards, particle system.
  - Support draw mulit-render, outline(stencil buffer) and multisample anti-aliasing(MASS)
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
    - "Up","Down","Left","Right","W","A","S","D","Q","E","Z","C","X","MouseLeftMove","MouseRightMove" : Move the camera.
    - "/" : turn on command mode or input directly in console.
      - "showmesh" : switch between mesh mode and model mode.
      - "shownormal" : switch if drawing normal or not.
      - "resetcamera" : Initialize the camera.
      - "scene [p1]" : Restart the programe. p1 is scene name. If p1 is empty, it means to restart the current scene. In config.xml, it has 28000, 28001, 28002, 28003.

# config.xml:
  - lots of setting.......

# ToDoList:
  - study 
    - https://github.com/SaschaWillems/Vulkan
    - https://learnopengl.com/
    - https://learnopengl-cn.github.io/
  - HDR, PBR, Shadow,
  - text render, UI 
  - release memory
  - check return
  - https://msdn.microsoft.com/en-us/library/windows/desktop/bb204633(v=vs.85).aspx
---
  - bounding box & sphere, ray, collision
  - Terrain system
  - parallel programming for collision and physical
  - select object
  - convert the whole assets into a single binary data
---
  - physical
  - combine socket https://github.com/KunyiLockeLin/WebSocket_server
  - frustum Culling
  - Dynamic Decals
  - specular map, image based lighting map, metallicRoughness map, occlusion map, emissive map.
  - mix animation
  - room
  - C-style
  - L-system, Swarm grammar
  - combine browser
  - live stream
  - machine learning for performance
  
# Bug
  - release mode crash
  
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
  - https://learnopengl.com/
  - https://learnopengl-cn.github.io/
  - http://www.opengl-tutorial.org/
  - http://lodev.org/lodepng/
  - https://github.com/KhronosGroup/glTF-WebGL-PBR
  - https://github.com/TheThinMatrix/OpenGL-Animation
  - https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt
  - https://learnopengl.com/#!Advanced-OpenGL/Advanced-GLSL
  - https://github.com/KhronosGroup/Vulkan-Docs/issues/274
  - https://github.com/chunying/gaminganywhere
  - https://gpuopen.com/
  - https://youtu.be/PxopE6Lwz3g
  - http://www.humus.name/
  
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

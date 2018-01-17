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
  - Comptuer Graphics's power is by Vulkan. VulkanSDK is a only third party llibrary.
  - Support mutli-viewports.
  - Support simple light and camera control.
  - Support vertex, geometry, and fragment shader.
  - Support Phong shading and PBR(Physically-Based Rendering)
  - Support diffuse/baseColor map.
  - Support skeletal animation(Only for gltf) and attach obj.
  - For model  - obj, mtl, gltf.
  - For image  - png, jpeg, bmp.
  - For string - xml, json.
  - Decode deflate, Huffman code, png, jpeg.
  - Math library.
  
# ToDoList:
  - post processing shader
  - HDR
  - bounding Box
  - collision
  - Terrian system
  - Dynamic Decals
  - normal map, specular map, image based lighting map, metallicRoughness map, occlusion map, emissive map.
  - shadow
  - reflection and cube map
  - mix animation
  - frustum Culling
  - room
  - combine socket https://github.com/KunyiLockeLin/WebSocket_server
  - separate to different many libraries
 
# Reference:
  - https://www.khronos.org/blog/beginners-guide-to-vulkan
  - https://renderdoc.org/vulkan-in-30-minutes.html
  - https://vulkan-tutorial.com/
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

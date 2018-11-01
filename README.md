# Queen Engine

  - Introduction video:

[![IMAGE ALT TEXT HERE](http://img.youtube.com/vi/lYOUqqthTN8/0.jpg)](https://youtu.be/lYOUqqthTN8)
  -  Even though, I call it a game engine, it is not just for game development. It is for my interest. I might do something that does not relate game development, also might not want to do something, even though it is really important for game development.
  
  - Scene: default 0
    - How to add and delete a scene, object, component,
    - and load eid for scene, object, component, 
    - and modify paraments and update the modification.
    - and set model, outline, position, material, show normal and mesh.

  ![alt text](https://github.com/KunyiLockeLin/QueenEngine_Vulkan/blob/master/sample01.gif)
  
  - Scene: scene1 1
    - How to set camera control and focus, use post-processing, and modify gamma and exposure.
    - Main featurn in this scene, cubemap, animation, attach skeletion, particle,
    - line, reflection & refraction, mulit-render, 2D render, bloom, grayscale.

  ![alt text](https://github.com/KunyiLockeLin/QueenEngine_Vulkan/blob/master/sample02.gif)
  
  - Scene: pbr1 2
    - The scene is for PBR and multi-lights demo.

  ![alt text](https://github.com/KunyiLockeLin/QueenEngine_Vulkan/blob/master/pbrDemo.jpg)
  
  - Scene: raytracing1 3
    - When raytracingDepth's value of camera component is larger than 0, it runs ray tracing.
    - The system only supoorts spheres. In transform component, it could edit the location and radius of the sphere. localScaleX is radius.
    - Material component is neccseary. It only supports baseColor and metallic. If metallic's valule is larger than 0, it will reflect the environment.

  ![alt text](https://github.com/KunyiLockeLin/QueenEngine_Vulkan/blob/master/raytracingDemo.jpg)
  
  - In qeheader.h, you can find out the whole flag and type.

# Feature
  - mutli-viewports, post-processing.
  - light and camera control.
  - vertex, tessellation control & evaluation(triangle-faces become opposite(cw)), geometry, fragment shader, compute shader(particles).
  - ~~Phong shading~~ and PBR(Physically-Based Rendering), gamma, exposure, grayscale, bloom.     
  - ~~diffuse map,~~ baseColor map, cubemap & reflection & refraction, normal map.
  - skeletal animation, attach obj, draw lines, billboards, particle system.
  - draw mulit-render, 2D render, outline(stencil buffer) and multisample anti-aliasing(MSAA)
  - Log output, call stack trace and command.
  - For model  - ~~obj, mtl~~, gltf.
  - For image  - png, jpeg, bmp.
  - For string - xml, json.
  - Decode deflate, Huffman code, png, jpeg.
  - Math library.
  - Setting: data/config.xml
  - Input:
    - "esc" : Close the program.
    - "+" : Add a new viewport.
    - "-" : Remove the last viewport.
    - ~~"1"~"9" : Choose a viewport to control its camera.~~
    - "Up","Down","Left","Right","W","A","S","D","Q","E","Z","C","X","MouseLeftMove","MouseRightMove" : Move the camera.
    - "/" : turn on command mode or input directly in console.
      - "showmesh" : switch between mesh mode and model mode.
      - "shownormal" : switch if drawing normal or not.
      - "resetcamera" : Initialize the camera.
      - "scene [p1]" : Restart the programe. p1 is scene name. If p1 is empty, it means to restart the current scene. In config.xml, it has 28000, 28001, 28002, 28003.

# ToDoList
  - Physically based rendering - IBL-image based lighting
    - http://www.pbr-book.org/
    - https://learnopengl.com/PBR/IBL/Diffuse-irradiance
    - https://github.com/SaschaWillems/Vulkan#-physically-based-rendering
  - Ray tracing - model, texture, lighting, refraction, optimization
    - https://github.com/SaschaWillems/Vulkan#04---ray-tracing
    - https://www.scratchapixel.com/
    - https://github.com/petershirley/raytracinginoneweekend
    - https://www.youtube.com/watch?v=Q1cuuepVNoY
  - optimize decode png and jpeg
  - Wiki
  - debuggin tool
  
# Bug
  - release mode crash sometimes.
  - lines flash sometimes.

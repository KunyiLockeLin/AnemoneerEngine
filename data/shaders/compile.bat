%VULKAN_SDK%/Bin32/glslangValidator.exe -u
%VULKAN_SDK%/Bin32/glslangValidator.exe -V base.vert -o basev.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V base.geom -o baseg.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V base.frag -o basef.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V cube.geom -o cubeg.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V cube.frag -o cubef.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V skeleton.vert -o skeletonv.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V phong.geom -o phongg.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V phong.frag -o phongf.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V billboard.geom -o billboardg.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V billboard.frag -o billboardf.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V pbr.geom -o pbrg.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V pbr.frag -o pbrf.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V postprocessing.vert -o postprocessingv.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V postprocessing.geom -o postprocessingg.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V postprocessing.frag -o postprocessingf.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V grayscale.frag -o grayscalef.spv
pause
%VULKAN_SDK%/Bin32/glslangValidator.exe -u
%VULKAN_SDK%/Bin32/glslangValidator.exe -V base.vert -o basev.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V skeleton.vert -o skeletonv.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V phong.geom -o phongg.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V phong.frag -o phongf.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V billboard.geom -o billboardg.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V billboard.frag -o billboardf.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V pbr.geom -o pbrg.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V pbr.frag -o pbrf.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V post.vert -o postv.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V post.frag -o postf.spv
pause
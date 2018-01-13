%VULKAN_SDK%/Bin32/glslangValidator.exe -u
%VULKAN_SDK%/Bin32/glslangValidator.exe -V base.vert -o basev.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V base.geom -o baseg.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V base.frag -o basef.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V billboard.geom -o billboardg.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V billboard.frag -o billboardf.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V pbr.vert -o pbrv.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V pbr.geom -o pbrg.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V pbr.frag -o pbrf.spv
pause
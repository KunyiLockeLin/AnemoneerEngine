%VULKAN_SDK%/Bin32/glslangValidator.exe -u
%VULKAN_SDK%/Bin32/glslangValidator.exe -V base.vert -o basev.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V base.tesc -o basetc.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V base.tese -o basete.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V base.geom -o baseg.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V base.frag -o basef.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V point.vert -o pointv.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V line.geom -o lineg.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V billboard.geom -o billboardg.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V texturev.frag -o texturevf.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V texturem.frag -o texturemf.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V cubemap.frag -o cubemapf.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V skeleton.vert -o skeletonv.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V toon.frag -o toonf.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V pbr.geom -o pbrg.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V pbr.frag -o pbrf.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V postprocessing.geom -o postprocessingg.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V postprocessing.frag -o postprocessingf.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V grayscale.frag -o grayscalef.spv
%VULKAN_SDK%/Bin32/glslangValidator.exe -V particle.comp -o particlec.spv
pause
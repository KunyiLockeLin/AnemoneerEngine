%VULKAN_SDK%/Bin/glslangValidator.exe -u
%VULKAN_SDK%/Bin/glslangValidator.exe -V base.vert -o basev.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V base.tesc -o basetc.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V base.tese -o basete.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V base.geom -o baseg.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V basem.frag -o basemf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V basev.frag -o basevf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V point.vert -o pointv.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V line.geom -o lineg.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V outline.geom -o outlineg.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V billboard.geom -o billboardg.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V triangle.geom -o triangleg.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V texturev.frag -o texturevf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V texturem.frag -o texturemf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V render.frag -o renderf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V shadow.frag -o shadowf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V cubemap.frag -o cubemapf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V cubemapreflect.frag -o cubemapreflectf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V skeleton.vert -o skeletonv.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V toon.frag -o toonf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V pbr.geom -o pbrg.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V pbr.frag -o pbrf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V postprocessing.geom -o postprocessingg.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V postprocessing.frag -o postprocessingf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V grayscale.frag -o grayscalef.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V particle.comp -o particlec.spv
pause
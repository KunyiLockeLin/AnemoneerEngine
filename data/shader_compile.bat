@echo on

set input=shaders\
set output=%input%spv\
echo %input%
echo %output%
mkdir %output%

%VULKAN_SDK%/Bin/glslangValidator.exe -h
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%base.vert -o %output%basev.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%base.tesc -o %output%basetc.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%base.tese -o %output%basete.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%base.geom -o %output%baseg.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%basem.frag -o %output%basemf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%basev.frag -o %output%basevf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%point.vert -o %output%pointv.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%line.geom -o %output%lineg.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%outline.geom -o %output%outlineg.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%billboard.geom -o %output%billboardg.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%triangle.geom -o %output%triangleg.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%texturev.frag -o %output%texturevf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%texturem.frag -o %output%texturemf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%texture.frag -o %output%texturef.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%cubemap.frag -o %output%cubemapf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%cubemapreflect.frag -o %output%cubemapreflectf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%skeleton.vert -o %output%skeletonv.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%toon.frag -o %output%toonf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%pbr.vert -o %output%pbrv.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%pbr.geom -o %output%pbrg.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%pbr.frag -o %output%pbrf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%postprocessing.geom -o %output%postprocessingg.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%b2d.geom -o %output%b2dg.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%postprocessing.frag -o %output%postprocessingf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%grayscale.frag -o %output%grayscalef.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%bloom.frag -o %output%bloomf.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%particle.comp -o %output%particlec.spv
%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%raytracing.comp -o %output%raytracingc.spv
pause

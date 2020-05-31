setlocal EnableDelayedExpansion

%VULKAN_SDK%/Bin/glslangValidator.exe -h

set input=%~dp0\
set output=%~dp0..\output\data\shader\
::echo input:%input%
::echo output:%output%

rmdir %output% /s/q
mkdir %output%

for /f "tokens=*" %%i in ('dir /b %input%') do (
    set checker=%%i
    set checker=!checker:~0,6!
    if !checker! NEQ header if !checker! NEQ spv (%VULKAN_SDK%/Bin/glslangValidator.exe -V %input%%%i -o %output%%%i.spv)
)

pause

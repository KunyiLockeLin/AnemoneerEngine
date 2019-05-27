@echo off

rmdir build /s/q
mkdir build
cd build
cmake -G "Visual Studio 15 Win64" ..
timeout 5
cmake --build . --config Debug
timeout 5
cmake --build . --config Release
timeout 5
start AngryEngine.sln

cd..
@echo | call data/shader_compile.bat
timeout 5

start AngryEngine_debug.exe
timeout 10
taskkill /F /im AngryEngine_debug.exe
timeout 5
start AngryEngine.exe
timeout 10
taskkill /F /im AngryEngine.exe

pause

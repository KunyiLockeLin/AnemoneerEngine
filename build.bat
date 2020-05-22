@echo | call shader_compiler.bat
timeout 3

rmdir build /s/q
mkdir build
cd build
cmake -G "Visual Studio 15 Win64" ..
timeout 3

cmake --build . --target lib_common --config Debug
timeout 3

cmake --build . --target exe_CodeGenerator --config Debug
timeout 3

cd ..
@echo | call code_generator.bat
timeout 3

cd build
cmake --build . --target exe_AngryEngine --config Debug
timeout 3

cd ..
cd build
start AngryEngine.sln
cd ..
cd output
start AngryEngine_debug.exe
::timeout 10
::taskkill /F /im AngryEngine_debug.exe

pause

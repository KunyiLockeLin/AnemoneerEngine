@echo | call shader/shader_compiler.bat
timeout 3

rmdir build /s/q
mkdir build
cd build
cmake -G "Visual Studio 17" -A x64 ..
timeout 3

cmake --build . --target lib_common --config Debug
timeout 3

cmake --build . --target exe_CodeGenerator --config Debug
timeout 3

cd ..
@echo | call code_generator/code_generator.bat
timeout 3

cd build
cmake --build . --target exe_AnemoneerEngine --config Debug
timeout 3

cd ..
cd build
start AnemoneerEngine.sln
cd ..
cd output
start AnemoneerEngine_debug.exe
::timeout 10
::taskkill /F /im AnemoneerEngine_debug.exe

pause

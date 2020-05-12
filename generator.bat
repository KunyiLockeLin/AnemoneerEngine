@echo | call output/data/shader_compile.bat
timeout 3

rmdir build /s/q
mkdir build
cd build
cmake -G "Visual Studio 15 Win64" ..

cmake --build . --target CodeGenerator --config Debug
timeout 3

cd ..
@echo | call code_generator.bat
timeout 3

cd build
cmake --build . --target AngryEngine --config Debug
timeout 3

cd ..
cd output
start AngryEngine_debug.exe
timeout 10
taskkill /F /im AngryEngine_debug.exe

start AngryEngine.sln

pause

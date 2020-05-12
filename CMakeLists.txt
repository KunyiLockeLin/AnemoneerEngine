cmake_minimum_required (VERSION 3.8)
ADD_DEFINITIONS(-D_UNICODE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
project (AngryEngine)

# lib Common
add_library(libCommon SHARED common/common.h common/template_define.h common/encode.cpp common/math.cpp common/asset.cpp common/log.cpp common/timer.cpp)

set_target_properties(libCommon PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
set_target_properties(libCommon PROPERTIES OUTPUT_NAME_DEBUG libCommon_debug)

#target_compile_features(libCommon PUBLIC cxx_std_14)

target_link_libraries(libCommon Dbghelp)
set(DEBUG_libCommon ${CMAKE_CURRENT_SOURCE_DIR}/build/Debug/libCommon_debug.dll)
set(RELEASE_libCommon ${CMAKE_CURRENT_SOURCE_DIR}/build/Release/libCommon.dll)
add_custom_command(TARGET libCommon POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<$<CONFIG:debug>:${DEBUG_libCommon}>$<$<CONFIG:release>:${RELEASE_libCommon}>" ${CMAKE_CURRENT_SOURCE_DIR}/output COMMENT "copy libCommon")

#lib ui
#add_library(libUI SHARED)
#lib GPU
#add_library(libGPU SHARED)

# CodeGenerator
add_executable(CodeGenerator code_generator.bat code_generator/code_generator.cpp)

set_target_properties(CodeGenerator PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
set_target_properties(CodeGenerator PROPERTIES OUTPUT_NAME_DEBUG CodeGenerator_debug)
set_target_properties(CodeGenerator PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $(SolutionDir)$(Configuration))

target_include_directories(CodeGenerator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_directories(CodeGenerator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/build)
target_link_libraries(CodeGenerator PUBLIC debug libCommon_debug optimized libCommon)

# AngryEngine
set(SRC_NONE build.bat output/data/shader_compile.bat output/data/config.xml)
set(SRC_ALGORITHM src/game_encode.cpp)
set(SRC_ALGORITHM_HEADER src/game_encode.h)
set(SRC_UI src/ui.cpp)
set(SRC_UI_HEADER src/ui.h)
set(SRC_VULKAN src/graphics.cpp src/vulkan.cpp)
set(SRC_VULKAN_HEADER src/graphics.h src/vulkan.h)

set(SRC_GAMEOBJECT  src/animation.cpp   src/axis.cpp    src/camera.cpp
                    src/cubemap.cpp src/grid.cpp
                    src/inputcontrol.cpp src/light.cpp  src/line.cpp
                    src/material.cpp    src/model.cpp   src/object.cpp
                    src/particle.cpp    src/plane.cpp   src/postprocessing.cpp
                    src/transform.cpp   src/rendersetting.cpp src/scene.cpp)

set(SRC_GAMEOBJECT_HEADER src/animation.h   src/axis.h    src/camera.h
                          src/cubemap.h src/grid.h
                          src/inputcontrol.h src/light.h  src/line.h
                          src/material.h    src/model.h   src/object.h
                          src/particle.h    src/plane.h   src/postprocessing.h
                          src/transform.h   src/rendersetting.h src/scene.h)

set(SRC_MANAGER src/game_asset.cpp  src/command.cpp src/engine.cpp
                src/global.cpp  src/objectmanager.cpp)

set(SRC_MANAGER_HEADER src/header.h        src/game_asset.h src/command.h
                       src/global.h         src/engine.h
                       src/objectmanager.h)

file(GLOB_RECURSE  SRC_SHADER output/data/shaders/*)
file(GLOB_RECURSE  SRC_SHADER_SPV output/data/shaders/spv/*)
file(GLOB_RECURSE  SRC_SHADER_HEADER output/data/shaders/header*)
file(GLOB_RECURSE  SRC_MODEL output/data/models/*.gltf)

list(REMOVE_ITEM SRC_SHADER ${SRC_SHADER_SPV})
list(REMOVE_ITEM SRC_SHADER ${SRC_SHADER_HEADER})

source_group(algorithm FILES ${SRC_ALGORITHM})
source_group(algorithm\\header FILES ${SRC_ALGORITHM_HEADER})
source_group(gameobject FILES ${SRC_GAMEOBJECT})
source_group(gameobject\\header FILES ${SRC_GAMEOBJECT_HEADER})
source_group(manager FILES ${SRC_MANAGER})
source_group(manager\\header FILES ${SRC_MANAGER_HEADER})
source_group(ui FILES ${SRC_UI})
source_group(ui\\header FILES ${SRC_UI_HEADER})
source_group(vulkan FILES ${SRC_VULKAN})
source_group(vulkan\\header FILES ${SRC_VULKAN_HEADER})
source_group(shader FILES ${SRC_SHADER})
source_group(shader\\header FILES ${SRC_SHADER_HEADER})
source_group(model FILES ${SRC_MODEL})
source_group("" FILES ${SRC_NONE})

add_executable(AngryEngine  ${SRC_ALGORITHM} ${SRC_ALGORITHM_HEADER} ${SRC_GAMEOBJECT} ${SRC_GAMEOBJECT_HEADER}
                       ${SRC_MANAGER} ${SRC_MANAGER_HEADER} ${SRC_UI} ${SRC_UI_HEADER} ${SRC_VULKAN}
                       ${SRC_VULKAN_HEADER} ${SRC_SHADER} ${SRC_SHADER_HEADER} ${SRC_MODEL} ${SRC_NONE})

set_target_properties(AngryEngine PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
set_target_properties(AngryEngine PROPERTIES OUTPUT_NAME_DEBUG AngryEngine_debug)
set_target_properties(AngryEngine PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)

target_include_directories(AngryEngine PUBLIC $ENV{VULKAN_SDK}/Include ${CMAKE_CURRENT_SOURCE_DIR})
target_link_directories(AngryEngine PUBLIC $ENV{VULKAN_SDK}/Lib  ${CMAKE_CURRENT_SOURCE_DIR}/build)
target_link_libraries(AngryEngine vulkan-1 debug libCommon_debug optimized libCommon)

set(DEBUG_AngryEngine ${CMAKE_CURRENT_SOURCE_DIR}/build/Debug/AngryEngine_debug.exe)
set(RELEASE_AngryEngine ${CMAKE_CURRENT_SOURCE_DIR}/build/Release/AngryEngine.exe)
add_custom_command(TARGET AngryEngine POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<$<CONFIG:debug>:${DEBUG_AngryEngine}>$<$<CONFIG:release>:${RELEASE_AngryEngine}>" ${CMAKE_CURRENT_SOURCE_DIR}/output COMMENT "copy AngryEngine")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT AngryEngine)

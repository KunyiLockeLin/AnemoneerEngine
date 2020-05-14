cmake_minimum_required (VERSION 3.8)
ADD_DEFINITIONS(-D_UNICODE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
project (AngryEngine)

# bat build
add_custom_target(bat_build SOURCES build.bat output/data/config.xml COMMAND cmd /c ${CMAKE_CURRENT_SOURCE_DIR}/build.bat COMMENT "run build.bat")


# bat shader_compiler
file(GLOB_RECURSE  SHADER shader/*)
file(GLOB_RECURSE  SHADER_HEADER shader/header*)
list(REMOVE_ITEM SHADER ${SHADER_HEADER})
source_group(shader FILES ${SHADER})
source_group(shader_header FILES ${SHADER_HEADER})
add_custom_target(bat_shader_compiler SOURCES shader_compiler.bat ${SHADER} ${SHADER_HEADER} COMMAND cmd /c ${CMAKE_CURRENT_SOURCE_DIR}/shader_compiler.bat COMMENT "run shader_compiler.bat")


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
add_executable(exeCodeGenerator code_generator/code_generator.cpp)

set_target_properties(exeCodeGenerator PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
set_target_properties(exeCodeGenerator PROPERTIES OUTPUT_NAME_DEBUG CodeGenerator_debug)
set_target_properties(exeCodeGenerator PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $(SolutionDir)$(Configuration))

target_include_directories(exeCodeGenerator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_directories(exeCodeGenerator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/build)
target_link_libraries(exeCodeGenerator PUBLIC debug libCommon_debug optimized libCommon)
#add_custom_command(TARGET exeCodeGenerator POST_BUILD COMMAND cmd //C code_generator.bat COMMENT "run code_generator.bat")
#add_custom_target(exeCodeGenerator COMMAND cmd /c ${CMAKE_CURRENT_SOURCE_DIR}/code_generator.bat COMMENT "run code_generator.bat")


# bat code_generator
add_custom_target(bat_code_generator SOURCES code_generator.bat COMMAND cmd /c ${CMAKE_CURRENT_SOURCE_DIR}/code_generator.bat COMMENT "run code_generator.bat")


# AngryEngine
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

file(GLOB_RECURSE  SRC_MODEL output/data/models/*.gltf)

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
source_group(model FILES ${SRC_MODEL})

add_executable(exeAngryEngine  ${SRC_ALGORITHM} ${SRC_ALGORITHM_HEADER} ${SRC_GAMEOBJECT} ${SRC_GAMEOBJECT_HEADER}
                       ${SRC_MANAGER} ${SRC_MANAGER_HEADER} ${SRC_UI} ${SRC_UI_HEADER} ${SRC_VULKAN}
                       ${SRC_VULKAN_HEADER} ${SRC_MODEL})

set_target_properties(exeAngryEngine PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
set_target_properties(exeAngryEngine PROPERTIES OUTPUT_NAME_DEBUG AngryEngine_debug)
set_target_properties(exeAngryEngine PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)

target_include_directories(exeAngryEngine PUBLIC $ENV{VULKAN_SDK}/Include ${CMAKE_CURRENT_SOURCE_DIR})
target_link_directories(exeAngryEngine PUBLIC $ENV{VULKAN_SDK}/Lib  ${CMAKE_CURRENT_SOURCE_DIR}/build)
target_link_libraries(exeAngryEngine vulkan-1 debug libCommon_debug optimized libCommon)

set(DEBUG_AngryEngine ${CMAKE_CURRENT_SOURCE_DIR}/build/Debug/AngryEngine_debug.exe)
set(RELEASE_AngryEngine ${CMAKE_CURRENT_SOURCE_DIR}/build/Release/AngryEngine.exe)
add_custom_command(TARGET exeAngryEngine POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<$<CONFIG:debug>:${DEBUG_AngryEngine}>$<$<CONFIG:release>:${RELEASE_AngryEngine}>" ${CMAKE_CURRENT_SOURCE_DIR}/output COMMENT "copy AngryEngine")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT exeAngryEngine)

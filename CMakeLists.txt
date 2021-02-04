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


# lib common
add_library(lib_common SHARED common/common.h common/template_define.h common/encode.cpp common/math.cpp common/asset.cpp common/log.cpp common/timer.cpp)

set_target_properties(lib_common PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
set_target_properties(lib_common PROPERTIES OUTPUT_NAME_DEBUG common_debug)
set_target_properties(lib_common PROPERTIES OUTPUT_NAME_RELEASE common)

#target_compile_features(lib_common PUBLIC cxx_std_14)

target_link_libraries(lib_common Dbghelp)
set(DEBUG_common ${CMAKE_CURRENT_SOURCE_DIR}/build/Debug/common_debug.dll)
set(RELEASE_common ${CMAKE_CURRENT_SOURCE_DIR}/build/Release/common.dll)
add_custom_command(TARGET lib_common POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<$<CONFIG:debug>:${DEBUG_common}>$<$<CONFIG:release>:${RELEASE_common}>" ${CMAKE_CURRENT_SOURCE_DIR}/output COMMENT "copy common")


#lib ui
#add_library(lib_ui SHARED)


#lib gpu
#add_library(lib_gpu SHARED)


# exe CodeGenerator
add_executable(exe_CodeGenerator code_generator/code_generator.cpp)

set_target_properties(exe_CodeGenerator PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
set_target_properties(exe_CodeGenerator PROPERTIES OUTPUT_NAME_DEBUG CodeGenerator_debug)
set_target_properties(exe_CodeGenerator PROPERTIES OUTPUT_NAME_RELEASE CodeGenerator)
set_target_properties(exe_CodeGenerator PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $(SolutionDir)$(Configuration))

target_include_directories(exe_CodeGenerator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_directories(exe_CodeGenerator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/build)
target_link_libraries(exe_CodeGenerator PUBLIC debug common_debug optimized common)
#add_custom_command(TARGET exe_CodeGenerator POST_BUILD COMMAND cmd //C code_generator.bat COMMENT "run code_generator.bat")
#add_custom_target(exe_CodeGenerator COMMAND cmd /c ${CMAKE_CURRENT_SOURCE_DIR}/code_generator.bat COMMENT "run code_generator.bat")


# bat code_generator
add_custom_target(bat_code_generator SOURCES code_generator.bat COMMAND cmd /c ${CMAKE_CURRENT_SOURCE_DIR}/code_generator.bat COMMENT "run code_generator.bat")


# exe AngryEngine
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

add_executable(exe_AngryEngine  ${SRC_ALGORITHM} ${SRC_ALGORITHM_HEADER} ${SRC_GAMEOBJECT} ${SRC_GAMEOBJECT_HEADER}
                                 ${SRC_MANAGER} ${SRC_MANAGER_HEADER} ${SRC_UI} ${SRC_UI_HEADER} ${SRC_VULKAN}
                                 ${SRC_VULKAN_HEADER} ${SRC_MODEL})

set_target_properties(exe_AngryEngine PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
set_target_properties(exe_AngryEngine PROPERTIES OUTPUT_NAME_DEBUG AngryEngine_debug)
set_target_properties(exe_AngryEngine PROPERTIES OUTPUT_NAME_RELEASE AngryEngine)
set_target_properties(exe_AngryEngine PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)

target_include_directories(exe_AngryEngine PUBLIC $ENV{VULKAN_SDK}/Include ${CMAKE_CURRENT_SOURCE_DIR})
target_link_directories(exe_AngryEngine PUBLIC $ENV{VULKAN_SDK}/Lib  ${CMAKE_CURRENT_SOURCE_DIR}/build)
target_link_libraries(exe_AngryEngine vulkan-1 debug common_debug optimized common)

set(DEBUG_AngryEngine ${CMAKE_CURRENT_SOURCE_DIR}/build/Debug/AngryEngine_debug.exe)
set(RELEASE_AngryEngine ${CMAKE_CURRENT_SOURCE_DIR}/build/Release/AngryEngine.exe)
add_custom_command(TARGET exe_AngryEngine POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<$<CONFIG:debug>:${DEBUG_AngryEngine}>$<$<CONFIG:release>:${RELEASE_AngryEngine}>" ${CMAKE_CURRENT_SOURCE_DIR}/output COMMENT "copy AngryEngine")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT exe_AngryEngine)

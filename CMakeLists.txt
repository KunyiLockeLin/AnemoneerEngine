cmake_minimum_required (VERSION 3.8)

ADD_DEFINITIONS(-D_UNICODE)

set(PROJECT_NAME "AngryEngine")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project (${PROJECT_NAME})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

set(SRC_NONE generator.bat data/shader_compile.bat data/config.xml)
set(SRC_ALGORITHM src/encode.h src/math.h src/encode.cpp src/math.cpp)
set(SRC_UI src/ui.h src/ui.cpp)
set(SRC_VULKAN src/graphics.h src/vulkan.h src/graphics.cpp src/vulkan.cpp)

set(SRC_GAMEOBJECT  src/animation.h   src/axis.h    src/camera.h
                    src/component.h   src/cubemap.h src/grid.h
                    src/inputcontrol.h src/light.h  src/line.h
                    src/material.h    src/model.h   src/object.h
                    src/particle.h    src/plane.h   src/postprocessing.h
                    src/transform.h   src/rendersetting.h src/scene.h
                    src/animation.cpp   src/axis.cpp    src/camera.cpp
                    src/component.cpp   src/cubemap.cpp src/grid.cpp
                    src/inputcontrol.cpp src/light.cpp  src/line.cpp
                    src/material.cpp    src/model.cpp   src/object.cpp
                    src/particle.cpp    src/plane.cpp   src/postprocessing.cpp
                    src/transform.cpp   src/rendersetting.cpp src/scene.cpp)

set(SRC_MANAGER src/header.h src/asset.h   src/command.h src/engine.h
                src/global.h  src/log.h
                src/objectmanager.h   src/timer.h
                src/asset.cpp   src/command.cpp src/engine.cpp
                src/global.cpp  src/log.cpp
                src/objectmanager.cpp   src/timer.cpp)

file(GLOB_RECURSE  SRC_SHADER data/shaders/*)
file(GLOB_RECURSE  SRC_SHADER_SPV data/shaders/spv/*)
file(GLOB_RECURSE  SRC_SHADER_HEADER data/shaders/header*)
file(GLOB_RECURSE  SRC_MODEL data/models/*.gltf)

list(REMOVE_ITEM SRC_SHADER ${SRC_SHADER_SPV})
list(REMOVE_ITEM SRC_SHADER ${SRC_SHADER_HEADER})

source_group(algorithm FILES ${SRC_ALGORITHM})
source_group(gameobject FILES ${SRC_GAMEOBJECT})
source_group(manager FILES ${SRC_MANAGER})
source_group(ui FILES ${SRC_UI})
source_group(vulkan FILES ${SRC_VULKAN})
source_group(shader FILES ${SRC_SHADER})
source_group(shader\\header FILES ${SRC_SHADER_HEADER})
source_group(model FILES ${SRC_MODEL})
source_group("" FILES ${SRC_NONE})

add_executable(${PROJECT_NAME}  ${SRC_ALGORITHM}    ${SRC_GAMEOBJECT}   ${SRC_MANAGER}
                                ${SRC_UI}           ${SRC_VULKAN}       ${SRC_SHADER}
                                ${SRC_SHADER_HEADER} ${SRC_MODEL}       ${SRC_NONE})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME_DEBUG ${PROJECT_NAME}_debug)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(${PROJECT_NAME} PUBLIC $ENV{VULKAN_SDK}/Include)
target_link_libraries(${PROJECT_NAME} PUBLIC $(VULKAN_SDK)/Lib/vulkan-1.lib Dbghelp.lib)

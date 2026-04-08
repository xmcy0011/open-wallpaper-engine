find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

set(LZ4_LINK_DEPS "")
set(LZ4_INCLUDE_DEPS "")
if(WIN32)
  find_package(lz4 CONFIG REQUIRED)
  list(APPEND LZ4_LINK_DEPS lz4::lz4)
else()
  pkg_check_modules(LZ4 REQUIRED liblz4)
  list(APPEND LZ4_LINK_DEPS ${LZ4_LIBRARIES})
  list(APPEND LZ4_INCLUDE_DEPS ${LZ4_INCLUDE_DIRS})
endif()

option(ENABLE_RENDERDOC "Build with renderdoc api" OFF)

if(ENABLE_RENDERDOC)
  add_compile_definitions(ENABLE_RENDERDOC_API=1)
endif()

include(TestBigEndian)
test_big_endian(ENDIAN)
if(ENDIAN)
  add_compile_definitions(WP_BIG_ENDIAN)
endif()

if(MSVC)
  set(warn_opts
      /W4 /permissive-
      CACHE INTERNAL "")
else()
  set(warn_opts
      -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wno-unused-variable
      CACHE INTERNAL "")
endif()

include_directories(.)

add_subdirectory(Fs)
add_subdirectory(Utils)
add_subdirectory(Timer)
add_subdirectory(Audio)
add_subdirectory(Scene)
add_subdirectory(Looper)
add_subdirectory(Vulkan)
add_subdirectory(Particle)
add_subdirectory(VulkanRender)
add_subdirectory(RenderGraph)

add_library(
  ${PROJECT_NAME} STATIC
  WPJson.cpp
  WPPkgFs.cpp
  Type.cpp
  wpscene/WPImageObject.cpp
  wpscene/WPParticleObject.cpp
  wpscene/WPMaterial.cpp
  wpscene/WPScene.cpp
  wpscene/WPLightObject.cpp
  WPParticleParser.cpp
  WPShaderParser.cpp
  WPSceneParser.cpp
  WPShaderValueUpdater.cpp
  WPTexImageParser.cpp
  WPSoundParser.cpp
  WPMdlParser.cpp
  WPPuppet.cpp
  SceneWallpaper.cpp)

set(InteralLib
    wpAudio
    wpLooper
    wpVulkanRender
    wpFs
    wpScene
    wpVulkan
    wpRGraph
    wpParticle
    wpTimer)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC wpUtils
  PRIVATE ${InteralLib} nlohmann_json ${LZ4_LINK_DEPS}
          ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})

target_include_directories(${PROJECT_NAME} PUBLIC . Swapchain ${LZ4_INCLUDE_DEPS})
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

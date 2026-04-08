set(LIB_NAME wpVulkan)

find_package(Vulkan REQUIRED)
add_compile_definitions(VK_USE_64_BIT_PTR_DEFINES=1)

add_library(${LIB_NAME}
STATIC
Instance.cpp
Device.cpp
GraphicsPipeline.cpp
Shader.cpp
StagingBuffer.cpp
Swapchain.cpp
TextureCache.cpp
Parameters.cpp
Vma.cpp
vulkan_wrapper.cpp
)

target_link_libraries(${LIB_NAME}
PUBLIC
	glslang
    spirv-reflect-static
	${Vulkan_LIBRARIES}
    wpScene
PRIVATE
	SPIRV
)
target_include_directories(
  ${LIB_NAME}
  BEFORE
  PUBLIC include ${Vulkan_INCLUDE_DIRS}
  PRIVATE include/Vulkan include/vkk)
target_include_directories(
  ${LIB_NAME}
  BEFORE
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/glslang)
target_compile_options(${LIB_NAME} PRIVATE ${warn_opts})
if(NOT MSVC)
  target_compile_options(${LIB_NAME} PRIVATE -Wno-missing-field-initializers)
endif()
set_property(TARGET ${LIB_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

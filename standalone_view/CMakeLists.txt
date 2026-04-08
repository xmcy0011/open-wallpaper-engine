cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project("sceneviewer")

set(DEFAULT_BUILD_TYPE "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
endif()

option(BUILD_QML "Build qml scene viewer" ${BUILD_QML})

include(FetchContent)

if(NOT WIN32)
    find_package(PkgConfig REQUIRED)
endif()

add_compile_options(
    $<$<AND:$<CONFIG:Debug>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-ggdb>
    $<$<AND:$<CONFIG:Debug>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-fno-omit-frame-pointer>
    $<$<AND:$<CONFIG:Debug>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-fsanitize=address>
)

add_link_options(
    $<$<AND:$<CONFIG:Debug>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-fsanitize=address>
)

include_directories(../third_party)
add_subdirectory(.. scenebackend)

set(GLFW_TARGET "")
if(WIN32)
    option(GLFW_USE_FETCHCONTENT "Download/build GLFW via FetchContent" OFF)
    # Prefer a prebuilt GLFW (vcpkg / system install). Fallback to FetchContent.
    find_package(glfw3 CONFIG QUIET)
    if(TARGET glfw)
        set(GLFW_TARGET glfw)
    elseif(TARGET glfw3)
        set(GLFW_TARGET glfw3)
    elseif(GLFW_USE_FETCHCONTENT)
        set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
        set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG        3.3.9
        )
        FetchContent_MakeAvailable(glfw)
        set(GLFW_TARGET glfw)
    else()
        message(FATAL_ERROR
            "GLFW not found. On Windows, install via vcpkg and reconfigure with the vcpkg toolchain, "
            "or set GLFW_USE_FETCHCONTENT=ON if you have network access.\n"
            "Example (vcpkg):\n"
            "  vcpkg install glfw3:x64-windows\n"
            "  cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake\n")
    endif()
else()
    pkg_check_modules(GLFW REQUIRED glfw3)
endif()

add_executable(${PROJECT_NAME}
    glfwviewer.cpp
)
find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME}
    PUBLIC 
        $<$<BOOL:${GLFW_TARGET}>:${GLFW_TARGET}>
        $<$<NOT:$<BOOL:${GLFW_TARGET}>>:${GLFW_LIBRARIES}>
        Vulkan::Vulkan
        wescene-renderer
)

if(NOT WIN32)
    target_include_directories(${PROJECT_NAME} PUBLIC ${GLFW_INCLUDE_DIRS})
endif()

if(BUILD_QML)
    find_package(Qt${QT_MAJOR_VERSION} COMPONENTS Qml REQUIRED)

    set(CMAKE_AUTOMOC ON) 
    set(CMAKE_AUTORCC ON) 
    set(CMAKE_AUTOUIC ON)

    add_executable(${PROJECT_NAME}-qml
        qmlviewer.cpp
        main.qml
        pkg.qrc
    )
    target_link_libraries(${PROJECT_NAME}-qml
        PUBLIC 
        wescene-renderer-qml
    )
    set(CMAKE_AUTOMOC OFF) 
    set(CMAKE_AUTORCC OFF) 
    set(CMAKE_AUTOUIC OFF)
endif()

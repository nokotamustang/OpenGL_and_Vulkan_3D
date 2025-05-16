cmake_minimum_required (VERSION 4.0)
project (app VERSION 1.0 LANGUAGES CXX)
include(FetchContent)

message(STATUS "building with ${CMAKE_BUILD_TYPE} configuration")

# Enable C++
enable_language(CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wno-pragmas -Wno-stringop-overflow")
endif()
# -g to enable debugging
# -O2 for optimization in speed, 01 for code size
# -Wextra and -Wall for extra warnings
# -Wno-pragmas to ignore pragmas warnings
# -Wno-stringop-overflow to ignore string overflow warnings

# Enable C
enable_language(C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wno-pragmas -Wno-stringop-overflow")
endif()

# FetchContent_Declare(
#     VulkanHppModule
#     GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Hpp.git
#     GIT_TAG v1.4.312
#     # GIT_TAG master
#     GIT_SHALLOW 1
# )
# FetchContent_MakeAvailable(VulkanHppModule)

include_directories(external)

# Include the GLFW library
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    # GIT_TAG 3.4
    GIT_TAG master
    GIT_SHALLOW 1
)
FetchContent_MakeAvailable(glfw)

# Include the FMT library
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    # GIT_TAG 11.1.4
    GIT_TAG master
    GIT_SHALLOW 1
)
FetchContent_MakeAvailable(fmt)

# GLM library
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG master
    GIT_SHALLOW 1
)
FetchContent_MakeAvailable(glm)

# Tiny Object Loader
FetchContent_Declare(
    tinyobjloader
    GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
    GIT_TAG v2.0.0rc13
    GIT_SHALLOW 1
)
FetchContent_MakeAvailable(tinyobjloader)

# Add an executable target called <name> to be built from the source files.
# This is the manual way of adding source files.
# add_executable(app
#     src/main.cpp
# )

# Add all source files in the src directory to the app executable.
file(GLOB_RECURSE CPP_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE C_SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)
add_executable(app ${CPP_SOURCES} ${C_SOURCES})

# Define a custom macro for build type
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  target_compile_definitions(app PRIVATE COMPILE_DEBUG=1)
endif()
if(CMAKE_BUILD_TYPE STREQUAL Development)
  target_compile_definitions(app PRIVATE COMPILE_DEVELOPMENT=1)
endif()
if(CMAKE_BUILD_TYPE STREQUAL Release)
  target_compile_definitions(app PRIVATE COMPILE_RELEASE=1)
endif()

target_include_directories(app PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    "../external/vulkan/include"
)

target_link_directories(app PRIVATE
    "../external/vulkan/lib"
)

target_link_libraries(app PRIVATE
    vulkan-1
    glfw
    fmt::fmt
    glm::glm
    tinyobjloader
)

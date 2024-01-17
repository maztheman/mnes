SET(SRCS
    KeyType.cpp
    PixelType.cpp
    PlatformType.cpp
    Timediff.cpp
)

add_library(GfxEngine STATIC ${SRCS})

add_subdirectory(Core)
add_subdirectory(Platform)

target_link_libraries(GfxEngine PRIVATE glad_lib glfw imgui_bindings glm::glm)
target_include_directories(GfxEngine PUBLIC ".")
target_compile_definitions(GfxEngine PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM)

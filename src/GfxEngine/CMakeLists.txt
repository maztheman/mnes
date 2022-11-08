find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

SET(SRCS
    KeyType.cpp
    PixelType.cpp
    PlatformType.cpp
    Timediff.cpp
)

add_library(GfxEngine STATIC ${SRCS})

add_subdirectory(Core)
add_subdirectory(Platform)

target_link_libraries(GfxEngine PRIVATE glad::glad glfw::glfw imgui::imgui glm::glm)
target_include_directories(GfxEngine PRIVATE "${CMAKE_BINARY_DIR}/bindings" PUBLIC ".")
target_compile_definitions(GfxEngine PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM)

include(cmake/CPM.cmake)

function(mnes_setup_dependencies)

if(NOT TARGET fmtlib::fmtlib)
    cpmaddpackage("gh:fmtlib/fmt#9.1.0")
endif()

if(NOT TARGET spdlog::spdlog)
    cpmaddpackage(
      NAME
      spdlog
      VERSION
      1.11.0
      GITHUB_REPOSITORY
      "gabime/spdlog"
      OPTIONS
      "SPDLOG_FMT_EXTERNAL ON")
endif()

if(NOT TARGET Catch2::Catch2WithMain)
    cpmaddpackage("gh:catchorg/Catch2@3.3.2")
endif()

if(NOT TARGET CLI11::CLI11)
    cpmaddpackage("gh:CLIUtils/CLI11@2.3.2")
endif()

cpmaddpackage("gh:Dav1dde/glad#v2.0.4")
add_subdirectory(${glad_SOURCE_DIR}/cmake)
glad_add_library(glad_lib STATIC API gl:core=3.3)

if (NOT TARGET GLFW::glfw)
    cpmaddpackage("gh:glfw/glfw#3.3.8")
endif()


if (NOT TARGET glm::glm)
    cpmaddpackage("gh:g-truc/glm#0.9.9.8")
endif()

if (NOT TARGET imgui)
    find_path(FT_INCLUDE_DIR freetype2/ft2build.h)
    cpmaddpackage("gh:ocornut/imgui#v1.89.8-docking")
    add_library(imgui_bindings STATIC)
    target_sources(imgui_bindings PRIVATE
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/misc/freetype/imgui_freetype.cpp
        ${imgui_SOURCE_DIR}/misc/fonts/binary_to_compressed_c.cpp
        ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
    )
    message("freetype 2 is: ${FT_INCLUDE_DIR}")
    target_include_directories(imgui_bindings PUBLIC ${imgui_SOURCE_DIR}/backends ${imgui_SOURCE_DIR} ${FT_INCLUDE_DIR}/freetype2)
endif()



endfunction()

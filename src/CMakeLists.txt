find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)


SET(SRCS 
  main.cpp
  processor.h
  processor.cpp
)


# Generic test that uses conan libs
add_executable(mnes ${SRCS})

target_link_libraries(
  mnes
  PUBLIC project_options project_warnings
  PRIVATE CLI11::CLI11 fmt::fmt spdlog::spdlog glad::glad glfw::glfw SDL2::SDL2)

target_include_directories(mnes PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include" ".")

target_precompile_headers(mnes PRIVATE "pch.h")

add_subdirectory(common)
add_subdirectory(cpu)
add_subdirectory(gfx)
add_subdirectory(mappers)
add_subdirectory(ppu)
add_subdirectory(sound)


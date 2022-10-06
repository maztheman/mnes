find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)


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
  PRIVATE CLI11::CLI11 fmt::fmt spdlog::spdlog glad::glad glfw::glfw SDL2::SDL2 glm::glm)

target_include_directories(mnes PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include" ".")

target_precompile_headers(mnes PRIVATE "pch.h")

add_subdirectory(common)
add_subdirectory(cpu)
add_subdirectory(gfx)
add_subdirectory(mappers)
add_subdirectory(ppu)
add_subdirectory(sound)

file(GLOB_RECURSE RUNTIME_FILES 
  "bnes.pal"
)

foreach(RT_FILE ${RUNTIME_FILES})
  get_filename_component(FILE_NAME ${RT_FILE} NAME)
  set(OUT_RT_FILE "${PROJECT_BINARY_DIR}/${FILE_NAME}")
  message("moving file ${RT_FILE} to ${OUT_RT_FILE}")
  add_custom_command(
    OUTPUT ${OUT_RT_FILE}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy "${RT_FILE}" "${OUT_RT_FILE}"
    DEPENDS ${RT_FILE})

  add_custom_command(TARGET mnes POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:mnes>/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${PROJECT_BINARY_DIR}/${FILE_NAME}"
        "$<TARGET_FILE_DIR:mnes>/${FILE_NAME}"
  )

  list(APPEND OUT_FILES ${OUT_RT_FILE})
endforeach(RT_FILE)

add_custom_target(
    RuntimeFiles 
    DEPENDS ${OUT_FILES}
)

add_dependencies(mnes RuntimeFiles)
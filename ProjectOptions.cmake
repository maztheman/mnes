include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


macro(mnes_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)
    set(SUPPORTS_UBSAN ON)
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    set(SUPPORTS_ASAN ON)
  endif()
endmacro()

macro(mnes_setup_options)
  option(mnes_ENABLE_HARDENING "Enable hardening" ON)
  option(mnes_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    mnes_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    mnes_ENABLE_HARDENING
    OFF)

  mnes_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR mnes_PACKAGING_MAINTAINER_MODE)
    option(mnes_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(mnes_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(mnes_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(mnes_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(mnes_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(mnes_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(mnes_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(mnes_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(mnes_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(mnes_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(mnes_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(mnes_ENABLE_PCH "Enable precompiled headers" OFF)
    option(mnes_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(mnes_ENABLE_IPO "Enable IPO/LTO" ON)
    option(mnes_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(mnes_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(mnes_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(mnes_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(mnes_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(mnes_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(mnes_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(mnes_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(mnes_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(mnes_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(mnes_ENABLE_PCH "Enable precompiled headers" OFF)
    option(mnes_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      mnes_ENABLE_IPO
      mnes_WARNINGS_AS_ERRORS
      mnes_ENABLE_USER_LINKER
      mnes_ENABLE_SANITIZER_ADDRESS
      mnes_ENABLE_SANITIZER_LEAK
      mnes_ENABLE_SANITIZER_UNDEFINED
      mnes_ENABLE_SANITIZER_THREAD
      mnes_ENABLE_SANITIZER_MEMORY
      mnes_ENABLE_UNITY_BUILD
      mnes_ENABLE_CLANG_TIDY
      mnes_ENABLE_CPPCHECK
      mnes_ENABLE_COVERAGE
      mnes_ENABLE_PCH
      mnes_ENABLE_CACHE)
  endif()

  mnes_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED AND (mnes_ENABLE_SANITIZER_ADDRESS OR mnes_ENABLE_SANITIZER_THREAD OR mnes_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(mnes_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(mnes_global_options)
  if(mnes_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    mnes_enable_ipo()
  endif()

  mnes_supports_sanitizers()

  if(mnes_ENABLE_HARDENING AND mnes_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR mnes_ENABLE_SANITIZER_UNDEFINED
       OR mnes_ENABLE_SANITIZER_ADDRESS
       OR mnes_ENABLE_SANITIZER_THREAD
       OR mnes_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${mnes_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${mnes_ENABLE_SANITIZER_UNDEFINED}")
    mnes_enable_hardening(mnes_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(mnes_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(mnes_warnings INTERFACE)
  add_library(mnes_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  mnes_set_project_warnings(
    mnes_warnings
    ${mnes_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(mnes_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    configure_linker(mnes_options)
  endif()

  include(cmake/Sanitizers.cmake)
  mnes_enable_sanitizers(
    mnes_options
    ${mnes_ENABLE_SANITIZER_ADDRESS}
    ${mnes_ENABLE_SANITIZER_LEAK}
    ${mnes_ENABLE_SANITIZER_UNDEFINED}
    ${mnes_ENABLE_SANITIZER_THREAD}
    ${mnes_ENABLE_SANITIZER_MEMORY})

  set_target_properties(mnes_options PROPERTIES UNITY_BUILD ${mnes_ENABLE_UNITY_BUILD})

  if(mnes_ENABLE_PCH)
    target_precompile_headers(
      mnes_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(mnes_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    mnes_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(mnes_ENABLE_CLANG_TIDY)
    mnes_enable_clang_tidy(mnes_options ${mnes_WARNINGS_AS_ERRORS})
  endif()

  if(mnes_ENABLE_CPPCHECK)
    mnes_enable_cppcheck(${mnes_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(mnes_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    mnes_enable_coverage(mnes_options)
  endif()

  if(mnes_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(mnes_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(mnes_ENABLE_HARDENING AND NOT mnes_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR mnes_ENABLE_SANITIZER_UNDEFINED
       OR mnes_ENABLE_SANITIZER_ADDRESS
       OR mnes_ENABLE_SANITIZER_THREAD
       OR mnes_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    mnes_enable_hardening(mnes_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()

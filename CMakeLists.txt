cmake_minimum_required(VERSION 3.31)

project(TouhouRPC
  VERSION 2.0.0
  LANGUAGES CXX
)

# Export compile commands for clangd integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default build options for all TouhouRPC executables and modules
function(touhourpc_build_options TARGET_NAME)
  # Use C++20 standard
  target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)
  
  # Compiler flags
  target_compile_options(${TARGET_NAME}
    PRIVATE
      /EHsc # Enable standard C++ exception handling
      /W4   # Generate warning levels 1-4
      /WX   # Treat warnings as errors
  )
endfunction()

# Dependencies
add_subdirectory(TouhouRPC)
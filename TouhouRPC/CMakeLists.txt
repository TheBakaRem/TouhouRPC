# Executable - TouhouRPC
add_executable(TouhouRPC)
target_sources(TouhouRPC PRIVATE TouhouRPC.cpp)
target_compile_features(TouhouRPC PRIVATE cxx_std_20)
target_link_libraries(TouhouRPC PRIVATE Discord Games Utils)

# Dependencies
add_subdirectory(discord)
add_subdirectory(games)
add_subdirectory(utils)
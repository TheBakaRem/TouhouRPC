# Executable - TouhouRPC
add_executable(TouhouRPC)
target_sources(TouhouRPC PRIVATE TouhouRPC.cpp)
touhourpc_build_options(TouhouRPC)
target_link_libraries(TouhouRPC PRIVATE Discord Games Utils)

# Dependencies
add_subdirectory(discord)
add_subdirectory(games)
add_subdirectory(utils)
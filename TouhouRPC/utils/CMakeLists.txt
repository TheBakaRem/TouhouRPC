# Include - Internal Utils
add_library(Utils STATIC)
target_compile_features(Utils PRIVATE cxx_std_20)
target_sources(Utils
  PRIVATE
    src/Config.cpp
    src/DiscordRPC.cpp
    src/GameDetector.cpp
    src/Log.cpp
    src/WindowsUtils.cpp
  PUBLIC FILE_SET HEADERS BASE_DIRS include FILES
    include/Config.hpp
    include/DiscordRPC.hpp
    include/GameDetector.hpp
    include/Log.hpp
    include/WindowsUtils.hpp
)
target_link_libraries(Utils PRIVATE Discord Games)
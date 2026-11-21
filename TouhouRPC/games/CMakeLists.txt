# Include - Touhou Games
add_library(Games STATIC)
target_compile_features(Games PRIVATE cxx_std_20)
target_sources(Games
  PRIVATE
    src/Touhou06.cpp
    src/Touhou07.cpp
    src/Touhou08.cpp
    src/Touhou09_5.cpp
    src/Touhou09.cpp
    src/Touhou10.cpp
    src/Touhou11.cpp
    src/Touhou12_5.cpp
    src/Touhou12_8.cpp
    src/Touhou12.cpp
    src/Touhou13.cpp
    src/Touhou14_3.cpp
    src/Touhou14.cpp
    src/Touhou15.cpp
    src/Touhou16.cpp
    src/Touhou17.cpp
    src/Touhou18.cpp
    src/TouhouBase.cpp
  PUBLIC FILE_SET HEADERS BASE_DIRS include FILES
    include/Enums.hpp
    include/Strings.hpp
    include/Touhou06.hpp
    include/Touhou07.hpp
    include/Touhou08.hpp
    include/Touhou09_5.hpp
    include/Touhou09.hpp
    include/Touhou10.hpp
    include/Touhou11.hpp
    include/Touhou12_5.hpp
    include/Touhou12_8.hpp
    include/Touhou12.hpp
    include/Touhou13.hpp
    include/Touhou14_3.hpp
    include/Touhou14.hpp
    include/Touhou15.hpp
    include/Touhou16.hpp
    include/Touhou17.hpp
    include/Touhou18.hpp
    include/TouhouBase.hpp
)
target_link_libraries(Games PRIVATE Discord Utils)
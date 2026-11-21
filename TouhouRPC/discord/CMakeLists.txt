# Include - Discord API
add_library(Discord STATIC)
target_compile_features(Discord PRIVATE cxx_std_20)
target_sources(Discord
  PRIVATE
    src/achievement_manager.cpp
    src/activity_manager.cpp
    src/application_manager.cpp
    src/core.cpp
    src/image_manager.cpp
    src/lobby_manager.cpp
    src/network_manager.cpp
    src/overlay_manager.cpp
    src/relationship_manager.cpp
    src/storage_manager.cpp
    src/store_manager.cpp
    src/types.cpp
    src/user_manager.cpp
    src/voice_manager.cpp
  PUBLIC FILE_SET HEADERS BASE_DIRS include FILES
    include/achievement_manager.h
    include/activity_manager.h
    include/application_manager.h
    include/core.h
    include/discord.h
    include/event.h
    include/ffi.h
    include/image_manager.h
    include/lobby_manager.h
    include/network_manager.h
    include/overlay_manager.h
    include/relationship_manager.h
    include/storage_manager.h
    include/store_manager.h
    include/types.h
    include/user_manager.h
    include/voice_manager.h
)
target_link_libraries(Discord PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libraries/x64/discord_game_sdk.dll.lib)
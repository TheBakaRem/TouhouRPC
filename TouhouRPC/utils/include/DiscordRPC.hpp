#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <string>

#include "discord.h"


class DiscordRPC {

public:
    DiscordRPC(std::int64_t clientID, int& lastErrorType);

    [[nodiscard]] bool isLaunched() const { return launched; }

    //! @brief Update the internal "instant submit" timers. Should be run every tick.
    discord::Result tickUpdate(int msDeltaTime);
    
    //! @brief Send the presence activity to Discord.
    //! @param forceSend Force the update without respecting rate-limit checks
    void sendPresence(bool forceSend);
    
    //! @brief Send an empty presence activity to Discord.
    void resetPresence();
    
    //! @brief Close the Discord application.
    void closeApp();

    //! @brief Set the presence activity data.
    void setActivityDetails(std::string const& details, std::string const& state, std::string const& largeIcon, std::string const& largeText, std::string const& smallIcon, std::string const& smallText);
    
    //! @brief Reset the presence activity timer (in-game time).
    void resetActivityTimeStartedToNow();

    //! @brief Send a warning log on error.
    static void showError(discord::Result res, int& lastErrorType);

    //! @brief Get a string representing the received Discord error.
    static std::string getErrorString(discord::Result res);

private:
    std::unique_ptr<discord::Core> core;
    discord::Activity activity;
    bool launched{ false };

    static const int MIN_TIME_BETWEEN_SUBMITS_MS{ 20000 };
    //! @brief Each instant submit has to tracked to make sure we don't go over rate limit.
    //! timeSinceLastSubmits.size() is the max we can do before we have to wait for one to be free.
    std::array<int, 3> timeSinceLastSubmits{ 0, 0, 0 };
};

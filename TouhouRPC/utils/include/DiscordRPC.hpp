#pragma once

#include "discord.h"
#include <memory>
#include <string>
#include <array>
#include <chrono>

#include "Log.hpp"


class DiscordRPC {

public:
    DiscordRPC(std::int64_t clientID, int& lastErrorType);

    bool isLaunched() const { return launched; }

    // main actions
    discord::Result tickUpdate(int msDeltaTime);
    void sendPresence(bool forceSend);
    void resetPresence();
    void closeApp();

    // set activity data
    void setActivityDetails(std::string const& details, std::string const& state, std::string const& largeIcon, std::string const& largeText, std::string const& smallIcon, std::string const& smallText);
    void resetActivityTimeStartedToNow();

    static void showError(discord::Result res, int& lastErrorType);
    static std::string getErrorString(discord::Result res);

private:
    std::unique_ptr<discord::Core> core;
    discord::Activity activity;
    bool launched{ false };

    // Each instant submit has to tracked to make sure we don't go over rate limit.
    // timeSinceLastSubmits.size() is the max we can do before we have to wait for one to be free.
    static const int MIN_TIME_BETWEEN_SUBMITS_MS{ 20000 };
    std::array<int, 3> timeSinceLastSubmits{ 0, 0, 0 };
};

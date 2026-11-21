#pragma once

#include "discord.h"
#include <memory>
#include <string>
#include <array>
#include <chrono>

#include "Log.hpp"

using namespace std;

class DiscordRPC {

public:
    DiscordRPC(int64_t clientID, int& lastErrorType);

    bool isLaunched() const { return launched; }

    // main actions
    discord::Result tickUpdate(int msDeltaTime);
    void sendPresence(bool forceSend);
    void resetPresence();
    void closeApp();

    // set activity data
    void setActivityDetails(string const& details, string const& state, string const& largeIcon, string const& largeText, string const& smallIcon, string const& smallText);
    void resetActivityTimeStartedToNow();

    static void showError(discord::Result res, int& lastErrorType);
    static string getErrorString(discord::Result res);

private:
    unique_ptr<discord::Core> core;
    discord::Activity activity;
    bool launched{ false };

    // Each instant submit has to tracked to make sure we don't go over rate limit.
    // timeSinceLastSubmits.size() is the max we can do before we have to wait for one to be free.
    static const int MIN_TIME_BETWEEN_SUBMITS_MS{ 20000 };
    array<int, 3> timeSinceLastSubmits{ 0, 0, 0 };
};

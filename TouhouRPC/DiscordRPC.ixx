module;

#include "includes/discord-files/discord.h"
#include <memory>
#include <string>
#include <array>
#include <chrono>

export module DiscordRPC;

import Log;

using namespace std;

export class DiscordRPC {

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

module : private;

// Constructor
// The lastErrorType value is used to tell if the error was the same at the last construction. If it is, it's not logged.
DiscordRPC::DiscordRPC(int64_t clientID, int& lastErrorType) {
    discord::Core* core_init;
    auto response{ discord::Core::Create(clientID, DiscordCreateFlags_NoRequireDiscord, &core_init) };

    if (!core_init) {
        DiscordRPC::showError(response, lastErrorType);
        launched = false;
    }
    else {
        core.reset(core_init);
        activity = discord::Activity{};
        timeSinceLastSubmits.fill(MIN_TIME_BETWEEN_SUBMITS_MS);
        resetActivityTimeStartedToNow(); // new discord app instance made so our "game time" starts from now
        Log::info("Discord client instantiated successfully!");
        launched = true;
    }
}

// Code that should run every tick
discord::Result DiscordRPC::tickUpdate(int msDeltaTime) {
    for (int& time : timeSinceLastSubmits) {
        time += msDeltaTime;
    }
    return core->RunCallbacks();
}

// Send the presence to Discord servers
void DiscordRPC::sendPresence(bool forceSend) {
    if (!forceSend) {
        bool cantSubmit{ true };
        for (int& time : timeSinceLastSubmits) {
            if (time >= MIN_TIME_BETWEEN_SUBMITS_MS) {
                cantSubmit = false;
                time = 0;
                break;
            }
        }
        if (cantSubmit) {
            return;
        }
    }
    core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
        if (result != discord::Result::Ok && result != discord::Result::TransactionAborted) {
            Log::warning("Failed updating RichPresence activity!");

            int err{ -1 };
            DiscordRPC::showError(result, err);
        }
        }
    );
}

// Reset presence
void DiscordRPC::resetPresence() {
    core->ActivityManager().ClearActivity([](discord::Result result) {
        if (result != discord::Result::Ok && result != discord::Result::TransactionAborted) {
            Log::warning("Failed clearing RichPresence activity!");

            int err{ -1 };
            DiscordRPC::showError(result, err);
        }
        }
    );
}

void DiscordRPC::closeApp() {
    core.reset();
    launched = false;
}

void DiscordRPC::setActivityDetails(string const& details, string const& state, string const& largeIcon, string const& largeText, string const& smallIcon, string const& smallText) {
    activity.SetDetails(details.c_str());
    activity.SetState(state.c_str());
    activity.GetAssets().SetLargeImage(largeIcon.c_str());
    activity.GetAssets().SetLargeText(largeText.c_str());
    activity.GetAssets().SetSmallImage(smallIcon.c_str());
    activity.GetAssets().SetSmallText(smallText.c_str());
}

void DiscordRPC::resetActivityTimeStartedToNow() {
    // discord timestamps expect seconds since epoch.
    activity.GetTimestamps().SetStart(chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count());
}

// Display a Discord error
void DiscordRPC::showError(discord::Result res, int& lastErrorType) {

    if (int(res) != lastErrorType) {
        Log::warning(getErrorString(res));
        lastErrorType = int(res);
    }
}

string DiscordRPC::getErrorString(discord::Result res) {
    string errType = "Discord ";

    switch (res) {
        case discord::Result::ServiceUnavailable:
            errType = "Service unavailable - Discord isn't working.";
            break;
        case discord::Result::InternalError:
            errType = "Internal error - Something went wrong on Discord's side.";
            break;
        case discord::Result::NotInstalled:
            errType = "Not installed - Discord is not installed.";
            break;
        case discord::Result::NotRunning:
            errType = "Not running - Discord is not running.";
            break;
        default: // All other cases
            errType = "An unexpected error occured. Error code: ";
            errType.append(to_string(static_cast<int>(res)));
    }

    return errType;
}


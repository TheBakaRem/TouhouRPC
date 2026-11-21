#include "DiscordRPC.hpp"

#include <chrono>
#include <cstdint>
#include <string>

#include "discord.h"
#include "Log.hpp"
#include "types.h"

// Constructor
// The lastErrorType value is used to tell if the error was the same at the last construction. If it is, it's not logged.
DiscordRPC::DiscordRPC(std::int64_t clientID, int& lastErrorType)
: activity() {
    discord::Core* core_init{ nullptr };
    const auto response{ discord::Core::Create(clientID, DiscordCreateFlags_NoRequireDiscord, &core_init) };
    
    if (core_init == nullptr) {
        DiscordRPC::showError(response, lastErrorType);
        launched = false;
    }
    else {
        core.reset(core_init);
        timeSinceLastSubmits.fill(MIN_TIME_BETWEEN_SUBMITS_MS);
        resetActivityTimeStartedToNow(); // new discord app instance made so our "game time" starts from now
        Log::info("Discord client instantiated successfully!");
        launched = true;
    }
}

discord::Result DiscordRPC::tickUpdate(int msDeltaTime) {
    for (int& time : timeSinceLastSubmits) {
        time += msDeltaTime;
    }
    return core->RunCallbacks();
}

void DiscordRPC::sendPresence(bool forceSend) {
    // Check and update rate limit timers
    bool isSubmittable{ false };
    for (int& time : timeSinceLastSubmits) {
        if (time >= MIN_TIME_BETWEEN_SUBMITS_MS) {
            isSubmittable = true;
            time = 0;
            break;
        }
    }

    // Update Discord presence activity if allowed or forced
    if (isSubmittable || forceSend) {
        core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
            if (result != discord::Result::Ok && result != discord::Result::TransactionAborted) {
                Log::warning("Failed updating RichPresence activity!");
                
                int err{ -1 };
                DiscordRPC::showError(result, err);
            }
        });
    }
}

void DiscordRPC::resetPresence() {
    core->ActivityManager().ClearActivity([](discord::Result result) {
        if (result != discord::Result::Ok && result != discord::Result::TransactionAborted) {
            Log::warning("Failed clearing RichPresence activity!");

            int err{ -1 };
            DiscordRPC::showError(result, err);
        }
    });
}

void DiscordRPC::closeApp() {
    core.reset();
    launched = false;
}

void DiscordRPC::setActivityDetails(std::string const& details, std::string const& state, std::string const& largeIcon, std::string const& largeText, std::string const& smallIcon, std::string const& smallText) {
    activity.SetDetails(details.c_str());
    activity.SetState(state.c_str());
    activity.GetAssets().SetLargeImage(largeIcon.c_str());
    activity.GetAssets().SetLargeText(largeText.c_str());
    activity.GetAssets().SetSmallImage(smallIcon.c_str());
    activity.GetAssets().SetSmallText(smallText.c_str());
}

void DiscordRPC::resetActivityTimeStartedToNow() {
    // discord timestamps expect seconds since epoch.
    activity.GetTimestamps().SetStart(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

void DiscordRPC::showError(discord::Result res, int& lastErrorType) {
    if (static_cast<int>(res) != lastErrorType) {
        Log::warning(getErrorString(res));
        lastErrorType = static_cast<int>(res);
    }
}

std::string DiscordRPC::getErrorString(discord::Result res) {
    std::string errType = "Discord ";

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
            errType.append(std::to_string(static_cast<int>(res)));
    }

    return errType;
}


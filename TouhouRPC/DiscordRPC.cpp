#include "discordRPC.h"

#include <iostream>
#include <cstdlib>
#include <string>
#include <chrono>

#include "Log.h"
using namespace std;

// Constructor
DiscordRPC::DiscordRPC(int64_t clientID)
{

	discord::Core* core{};
	auto response = discord::Core::Create(clientID, DiscordCreateFlags_NoRequireDiscord, &core);

	if (response == discord::Result::NotInstalled) {
		printLog(LOG_ERROR, "You do not have the Discord client installed on this PC. Install it before running this program.");
		logExit();
		std::exit(-1);
	}

	this->core.reset(core);

	if (!core) {
		printLog(LOG_ERROR, "Failed to instantiate Discord!");
		DiscordRPC::showError(response);
		launched = false;
	}
	else {
		this->activity = discord::Activity{};
		timeSinceLastSubmits.fill(MIN_TIME_BETWEEN_SUBMITS_MS);
		resetActivityTimeStartedToNow(); // new discord app instance made so our "game time" starts from now
		printLog(LOG_INFO, "Discord instantiated successfully!");
		launched = true;
	}

}

// Destructor
DiscordRPC::~DiscordRPC()
{
}

// Code that should run every tick
discord::Result DiscordRPC::tickUpdate(int msDeltaTime)
{
	for (unsigned int& time : timeSinceLastSubmits)
	{
		time += msDeltaTime;
	}
	return core->RunCallbacks();
}

// Send the presence to Discord servers
void DiscordRPC::sendPresence(bool forceSend)
{
	if (!forceSend)
	{
		bool cantSubmit = true;
		for (unsigned int& time : timeSinceLastSubmits)
		{
			if (time >= MIN_TIME_BETWEEN_SUBMITS_MS)
			{
				cantSubmit = false;
				time = 0;
				break;
			}
		}
		if (cantSubmit)
		{
			return;
		}
	}
	this->core->ActivityManager().UpdateActivity(this->activity, [](discord::Result result) {
		if (result != discord::Result::Ok && result != discord::Result::TransactionAborted)
		{
			printLog(LOG_WARNING, "Failed updating RichPresence activity!");
			DiscordRPC::showError(result);
		}
	});
}


// Reset presence
void DiscordRPC::resetPresence()
{
	this->core->ActivityManager().ClearActivity([](discord::Result result) {
		if (result != discord::Result::Ok && result != discord::Result::TransactionAborted)
		{
			printLog(LOG_WARNING, "Failed clearing RichPresence activity!");
			DiscordRPC::showError(result);
		}
	});
}

void DiscordRPC::closeApp()
{
	core = {};
	launched = false;
}

void DiscordRPC::setActivityDetails(std::string const& details, std::string const& state, std::string const& largeIcon, std::string const& largeText, std::string const& smallIcon, std::string const& smallText)
{
	activity.SetDetails(details.c_str());
	activity.SetState(state.c_str());
	activity.GetAssets().SetLargeImage(largeIcon.c_str());
	activity.GetAssets().SetLargeText(largeText.c_str());
	activity.GetAssets().SetSmallImage(smallIcon.c_str());
	activity.GetAssets().SetSmallText(smallText.c_str());
}

void DiscordRPC::resetActivityTimeStartedToNow()
{
	// discord timestamps expect seconds since epoch.
	activity.GetTimestamps().SetStart(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

// Display a Discord error
void DiscordRPC::showError(discord::Result res)
{
	string errType = "";

	switch (int(res)) {
	case 1: // Service unavailable
		errType = "Service unavailable - Discord isn't working.";
		break;
	case 26: // Not installed
		errType = "Not installed - Discord is not installed.";
		break;
	case 27: // Not running
		errType = "Not running - Discord is not running.";
		break;
	default: // All other cases
		errType = "An unexpected error occured. Error code: ";
		errType.append(to_string(int(res)));
	}

	printLog(LOG_WARNING, "Discord error code: %s", errType.c_str());
}

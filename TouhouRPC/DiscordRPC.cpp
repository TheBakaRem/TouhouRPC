#include "discordRPC.h"

#include <iostream>
#include <cstdlib>
#include <string>
#include <chrono>
using namespace std;

// Constructor
DiscordRPC::DiscordRPC(int64_t clientID)
{

	discord::Core* core{};
	auto response = discord::Core::Create(clientID, DiscordCreateFlags_NoRequireDiscord, &core);

	if (response == discord::Result::NotInstalled) {
		std::cout << "You do not have Discord installed on this PC. Install it before running this program." << std::endl;
		std::exit(-1);
	}

	this->core.reset(core);

	if (!core) {
		cout << "Failed to instantiate Discord!" << endl;
		DiscordRPC::showError(response);
		launched = false;
	}
	else {
		this->activity = discord::Activity{};
		resetActivityTimeStartedToNow(); // new discord app instance made so our "game time" starts from now
		cout << "Discord instantiated successfully!" << endl;
		launched = true;
	}

}

// Destructor
DiscordRPC::~DiscordRPC()
{
}

// Code that should run every tick
discord::Result DiscordRPC::tickUpdate()
{
	return core->RunCallbacks();
}

// Send the presence to Discord servers
void DiscordRPC::sendPresence()
{
	this->core->ActivityManager().UpdateActivity(this->activity, [](discord::Result result) {
		if (result != discord::Result::Ok && result != discord::Result::TransactionAborted)
		{
			cout << "Failed updating activity!" << endl;
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
			cout << "Failed clearing activity!" << endl;
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

	std::cerr << "Discord error: " << errType << std::endl;
}

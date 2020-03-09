#include "discordRPC.h"

#include <iostream>
#include <cstdlib>
#include <string>
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
		launched = false;
	}
	else {
		this->activity = discord::Activity{};
		cout << "Discord instantiated successfully!" << endl;
		launched = true;
	}

}

// Destructor
DiscordRPC::~DiscordRPC()
{
}

void DiscordRPC::setActivityDetails(std::string& details, std::string& state, std::string& largeIcon, std::string& largeText, std::string& smallIcon, std::string& smallText)
{
	activity.SetDetails(details.c_str());
	activity.SetState(state.c_str());
	activity.GetAssets().SetLargeImage(largeIcon.c_str());
	activity.GetAssets().SetLargeText(largeText.c_str());
	activity.GetAssets().SetSmallImage(smallIcon.c_str());
	activity.GetAssets().SetSmallText(smallText.c_str());
}


// Get an instance of the Core
std::shared_ptr<discord::Core> DiscordRPC::getCore()
{
	return this->core;
}

bool DiscordRPC::isLaunched()
{
	return this->launched;
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

// Send the presence to Discord servers
void DiscordRPC::sendPresence()
{
	this->core->ActivityManager().UpdateActivity(this->activity, [](discord::Result result) {
		if (result != discord::Result::Ok) cout << "Failed updating activity!" << endl;
	});
}


// Reset presence
void DiscordRPC::resetPresence()
{
	this->core->ActivityManager().ClearActivity([](discord::Result result) {
		if (result != discord::Result::Ok) cout << "Failed clearing activity!" << endl;
	});
}
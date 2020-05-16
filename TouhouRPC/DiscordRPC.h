
#ifndef DISCORDRPC_H
#define DISCORDRPC_H

#include "includes/discord-rpc/discord.h"
#include <cstdint>
#include <memory>
#include <string>

class DiscordRPC {

public:
	DiscordRPC(int64_t clientID);
	~DiscordRPC();

	bool isLaunched() const { return launched; }

	// main actions
	discord::Result tickUpdate();
	void sendPresence();
	void resetPresence();
	void closeApp();

	// set activity data
	void setActivityDetails(std::string const& details, std::string const& state, std::string const& largeIcon, std::string const& largeText, std::string const& smallIcon, std::string const& smallText);
	void resetActivityTimeStartedToNow();

	static void showError(discord::Result res);


private:
	std::shared_ptr<discord::Core> core;
	discord::Activity activity{};
	bool launched = false;
};

#endif // !DISCORDRPC_H

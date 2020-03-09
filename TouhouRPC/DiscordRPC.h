
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

	void setActivityDetails(std::string & details, std::string & state, std::string & largeIcon, std::string & largeText, std::string & smallIcon, std::string & smallText);


	std::shared_ptr<discord::Core> getCore();
	bool isLaunched();

	void showError(discord::Result res);
	void sendPresence();
	void resetPresence();

private:
	std::shared_ptr<discord::Core> core;
	discord::Activity activity{};
	bool launched = false;
};

#endif // !DISCORDRPC_H

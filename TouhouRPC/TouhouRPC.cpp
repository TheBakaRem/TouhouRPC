// TouhouRPC.cpp : Main file

// Copyright (C) 2020-2024  TheBakaRem
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.


// Includes
#include <chrono>
#include <csignal> // SIGINT
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <locale>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <Windows.h>

#include "Config.hpp"
#include "discord.h"
#include "DiscordRPC.hpp"
#include "GameDetector.hpp"
#include "Log.hpp"
#include "TouhouBase.hpp"

// Version definition
constexpr auto VERSION{ "2.0.0-beta1" };

namespace {
    // used to avoid repeating the same error message twice
    int discordLastErrorType{ -1 };

    DiscordRPC getDiscord(int64_t clientID) {
        Log::info("Connecting to the Discord client...");

        DiscordRPC discordClient{ clientID, discordLastErrorType };

        while (!discordClient.isLaunched()) {
            std::this_thread::sleep_for(std::chrono::seconds{ 1 });
            discordClient = DiscordRPC(clientID, discordLastErrorType);
        }
        discordLastErrorType = -1; // Reset the value
        return discordClient;
    }

    std::unique_ptr<TouhouBase> getTouhouGame() {
        Log::info("Waiting for a supported game to start...");

        std::unique_ptr<TouhouBase> ptrThBase{ initializeTouhouGame(false) };

        while (ptrThBase == nullptr) {
            std::this_thread::sleep_for(std::chrono::seconds{ 1 });
            ptrThBase = initializeTouhouGame(true);
        }
        return ptrThBase;
    }


    bool touhouUpdate(const std::unique_ptr<TouhouBase>& touhouGame, DiscordRPC& discord) {
        if (touhouGame->isStillRunning()) {
            // Update data in Touhou class
            touhouGame->readDataFromGameProcess();
            const bool stateHasChanged{ touhouGame->stateHasChangedSinceLastCheck() };

            // Get data from Touhou class
            std::string gameName;
            touhouGame->setGameName(gameName);

            std::string gameInfo;
            touhouGame->setGameInfo(gameInfo);

            std::string largeImageIcon;
            std::string largeImageText;
            touhouGame->setLargeImageInfo(largeImageIcon, largeImageText);

            std::string smallImageIcon;
            std::string smallImageText;
            touhouGame->setSmallImageInfo(smallImageIcon, smallImageText);

            // Update RPC data, always needs to run
            discord.setActivityDetails(gameName, gameInfo, largeImageIcon, largeImageText, smallImageIcon, smallImageText);

            return stateHasChanged;
        }
        return false;
    }

    // This code runs when a close event is detected (SIGINT or CTRL_CLOSE_EVENT)
    void programClose() {

    }


    BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType) {
        if (dwCtrlType == CTRL_CLOSE_EVENT) {
            programClose();
            return TRUE;
        }
        return FALSE;
    }

    void startDisplay() {
        std::cout << "TouhouRPC - Discord Rich Presence status for Touhou games - Version " << VERSION << "\n"
            "Available on GitHub: https://www.github.com/TheBakaRem/TouhouRPC\n"
            "Copyright (C) 2020-2024  TheBakaRem\n"
            "Licensed under the GNU General Public License v3.0. This program comes with ABSOLUTELY NO WARRANTY; for details see the LICENSE file.\n"
            "This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License.\n"
            "\n"
            "Usage: Once started, the program will automatically attach to a Touhou game currently running on the computer.\n"
            "The program automatically detects when you change between supported games.\n"
            "You can close this program at any time by pressing (Ctrl+C).\n"
            "Supported games: Touhou 06 (EoSD), 07 (PCB), 08 (IN), 09 (PoFV), 09.5 (StB), 10 (MoF), 11 (SA), 12 (UFO), 12.5 (DS), 12.8 (GFW), 13 (TD), 14 (DDC), 14.3 (ISC), 15 (LoLK), 16 (HSiFS), 17 (WBaWC), 18 (UM).\n"
            "\n"
            "!!THIS PROGRAM MIGHT BE ABLE TO TRIGGER ANTI-CHEAT SYSTEMS FROM OTHER GAMES, USE AT YOUR OWN RISK!!\n";
    }

} // namespace
int main() {
    // Program start and SIGINT (Ctrl+C) handler
    startDisplay();

    std::signal(SIGINT, [](int) {
        programClose();
        std::exit(SIGINT);
        }
    );

    SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE);

    // Force locale to be "en_US.UTF8".
    // This forces numbers to be displayed with the "," thousands separator.
    // This makes sense since this is an English-only program.
    std::locale::global(std::locale("en_US.UTF8"));

    // Config reading and log system initialization
    try {
        Config::parseFile();
        const int activateLogFiles{ Config::getValue("activateLogFiles") };
        const Log::LogLevel logLevelConsole{ static_cast<Log::LogLevel>(Config::getValue("logLevelConsole")) };
        const Log::LogLevel logLevelLogFile{ static_cast<Log::LogLevel>(Config::getValue("logLevelLogFile")) };

        if (activateLogFiles == 1) {
            Log::openLogFile();
            Log::setLogLevelFile(Log::LogLevel{ logLevelLogFile });
        }

        Log::setLogLevelConsole(Log::LogLevel{ logLevelConsole });
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what();
        return 1;
    }


    // GET TOUHOU GAME
    std::unique_ptr<TouhouBase> touhouGame{ getTouhouGame() };

    // GET DISCORD LINK
    DiscordRPC discord{ getDiscord(touhouGame->getClientId()) };
    if (!discord.isLaunched()) {
        Log::error("Discord is not running. Exiting program...");
        return -1;
    }

    Log::debug("Starting Discord Rich Presence display...");

    // MAIN LOOP

    constexpr int msBetweenTicks{ 500 };
    constexpr int ticksPerSecond{ 1000 / msBetweenTicks };
    constexpr int ticksBetweenRegularUpdates{ 16 };
    int regularUpdateTickCount{ ticksBetweenRegularUpdates };

    while (true) {
        const discord::Result discordResult{ discord.tickUpdate(msBetweenTicks) };

        if (discordResult != discord::Result::Ok) {
            DiscordRPC::showError(discordResult, discordLastErrorType);
            discord = getDiscord(touhouGame->getClientId());

            if (touhouUpdate(touhouGame, discord) && touhouGame->isStillRunning()) {
                discord.sendPresence(true);
            }
        }
        else {
            bool forceSend{ regularUpdateTickCount >= ticksBetweenRegularUpdates };
            const bool touhouUpdated{ touhouUpdate(touhouGame, discord) };
            const bool running{ touhouGame->isStillRunning() };
            if (running && (touhouUpdated || forceSend)) {
                // if within 1 second of a normal update, make it a normal update instead of wasting an instant slot
                if (touhouUpdated && (ticksBetweenRegularUpdates - regularUpdateTickCount) <= ticksPerSecond) {
                    forceSend = true;
                }
                discord.sendPresence(forceSend);
                if (forceSend) {
                    regularUpdateTickCount = 0;
                }
            }
            regularUpdateTickCount++;
        }

        if (!touhouGame->isStillRunning()) {

            // Presence reset
            discord.resetPresence();
            discord.closeApp();

            Log::info("Game closed. Ready to find another supported game.");

            touhouGame = getTouhouGame();
            discord = getDiscord(touhouGame->getClientId());

            Log::debug("Starting Discord Rich Presence display...");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds{ msBetweenTicks });
    }
}
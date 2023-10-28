// TouhouRPC.cpp : Main file

// Copyright (C) 2020  Rem
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

// Version definition
constexpr auto VERSION{ "1.5.0" };

// Includes
import <iostream>;
#include <thread>
import <chrono>;
#include <csignal>
#include <Windows.h>

import Config;
import Log;
import DiscordRPC;
import GameDetector;
import Games;

using namespace std;

// used to avoid repeating the same error message twice
int discordLastErrorType{ -1 };

DiscordRPC getDiscord(int64_t clientID) {
    Log::info("Connecting to the Discord client...");

    DiscordRPC d{ clientID, discordLastErrorType };

    while (!d.isLaunched()) {
        this_thread::sleep_for(1s);
        d = DiscordRPC(clientID, discordLastErrorType);
    }
    discordLastErrorType = -1; // Reset the value
    return d;
}

unique_ptr<TouhouBase> getTouhouGame() {
    Log::info("Waiting for a supported game to start...");

    unique_ptr<TouhouBase> d{ initializeTouhouGame(false) };

    while (d == nullptr) {
        this_thread::sleep_for(1s);
        d = initializeTouhouGame(true);
    }
    return d;
}

bool touhouUpdate(unique_ptr<TouhouBase>& touhouGame, DiscordRPC& discord) {
    if (touhouGame->isStillRunning()) {
        // Update data in Touhou class
        touhouGame->readDataFromGameProcess();
        const bool stateHasChanged{ touhouGame->stateHasChangedSinceLastCheck() };

        // Get data from Touhou class
        string gameName;
        touhouGame->setGameName(gameName);

        string gameInfo;
        touhouGame->setGameInfo(gameInfo);

        string largeImageIcon;
        string largeImageText;
        touhouGame->setLargeImageInfo(largeImageIcon, largeImageText);

        string smallImageIcon;
        string smallImageText;
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
    if (int(dwCtrlType) == CTRL_CLOSE_EVENT) {
        programClose();
        return TRUE;
    }
    return FALSE;
}

void startDisplay() {
    // just to spam logs in peace
    ios_base::sync_with_stdio(false);
    cout << "TouhouRPC - Discord Rich Presence status for Touhou games - Version " << VERSION << "\n"
        << "Available on GitHub: https://www.github.com/TheBakaRem/TouhouRPC\n"
        << "Copyright (C) 2020-2023  Rem\n"
        << "Licensed under the GNU General Public License v3.0. This program comes with ABSOLUTELY NO WARRANTY; for details see the LICENSE file.\n"
        << "This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License.\n"
        << "\n"
        << "Usage: Once started, the program will automatically attach to a Touhou game currently running on the computer.\n"
        << "The program automatically detects when you change between supported games.\n"
        << "You can close this program at any time by pressing (Ctrl+C).\n"
        << "Supported games: Touhou 06 (EoSD), 07 (PCB), 08 (IN), 09 (PoFV), 09.5 (StB), 10 (MoF), 11 (SA), 12 (UFO), 12.5 (DS), 12.8 (GFW), 13 (TD), 14 (DDC), 14.3 (ISC), 15 (LoLK), 16 (HSiFS), 17 (WBaWC), 18 (UM).\n"
        << "\n"
        << "!!THIS PROGRAM MIGHT BE ABLE TO TRIGGER ANTI-CHEAT SYSTEMS FROM OTHER GAMES, USE AT YOUR OWN RISK!!\n";
}

int main() {
    // Program start and SIGINT (Ctrl+C) handler
    startDisplay();

    signal(SIGINT, [](int) {
        programClose();
        exit(SIGINT);
        }
    );

    SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE);

    // Config reading and log system initialization
    try {
        Config::parseFile();
        const int activateLogFiles{ Config::getValue("activateLogFiles") };
        const int logLevelConsole{ Config::getValue("logLevelConsole") };
        const int logLevelLogFile{ Config::getValue("logLevelLogFile") };

        if (activateLogFiles == 1) {
            Log::openLogFile();
            Log::setLogLevelFile(Log::LogLevel{ logLevelLogFile });
        }

        Log::setLogLevelConsole(Log::LogLevel{ logLevelConsole });
    }
    catch (const runtime_error& e) {
        cerr << e.what();
        exit(1);
    }


    // GET TOUHOU GAME
    unique_ptr<TouhouBase> touhouGame{ getTouhouGame() };

    // GET DISCORD LINK
    DiscordRPC discord{ getDiscord(touhouGame->getClientId()) };
    if (!discord.isLaunched()) {
        Log::error("Discord is not running. Exiting program...");
        exit(-1);
    }

    Log::debug("Starting Discord Rich Presence display...");

    // MAIN LOOP

    constexpr int msBetweenTicks{ 500 };
    constexpr int ticksPerSecond{ 1000 / msBetweenTicks };
    constexpr int ticksBetweenRegularUpdates{ 16 };
    int regularUpdateTickCount{ ticksBetweenRegularUpdates };

    while (true) {
        discord::Result res{ discord.tickUpdate(msBetweenTicks) };

        if (res != discord::Result::Ok) {
            DiscordRPC::showError(res, discordLastErrorType);
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

        this_thread::sleep_for(chrono::milliseconds{ msBetweenTicks });
    }
}
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
#define VERSION_MAJOR 1
#define VERSION_MINOR 5
#define VERSION_REVISION 0

// Includes
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>

#include "Config.h"
#include "Log.h"
#include "DiscordRPC.h"
#include "GameDetector.h"
#include "games/TouhouBase.h"

using namespace std;

namespace {
    volatile bool interrupted{ false };
    int discordLastErrorType{ -1 };
}

DiscordRPC getDiscord(int64_t clientID) {
    logSystem->print(Log::LOG_INFO, "Connecting to the Discord client...");
    
    DiscordRPC d = DiscordRPC(clientID, discordLastErrorType);

    while (!d.isLaunched()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        d = DiscordRPC(clientID, discordLastErrorType);
    }
    discordLastErrorType = -1; // Reset the value
    return d;
}

std::unique_ptr<TouhouBase> getTouhouGame() {
    logSystem->print(Log::LOG_INFO, "Waiting for a supported game to start...");

    std::unique_ptr<TouhouBase> d = initializeTouhouGame(false);

    while (d == nullptr) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        d = initializeTouhouGame(true);
    }
    return d;
}

bool touhouUpdate(std::unique_ptr<TouhouBase>& touhouGame, DiscordRPC& discord)
{
    if (touhouGame->isStillRunning())
    {
        // Update data in Touhou class
        touhouGame->readDataFromGameProcess();
        bool const stateHasChanged = touhouGame->stateHasChangedSinceLastCheck();

        // Get data from Touhou class
        string gameName;
        string gameInfo;
        string largeImageIcon;
        string largeImageText;
        string smallImageIcon;
        string smallImageText;

        touhouGame->setGameName(gameName);
        touhouGame->setGameInfo(gameInfo);
        touhouGame->setLargeImageInfo(largeImageIcon, largeImageText);
        touhouGame->setSmallImageInfo(smallImageIcon, smallImageText);

        // Update RPC data, always needs to run
        discord.setActivityDetails(gameName, gameInfo, largeImageIcon, largeImageText, smallImageIcon, smallImageText);

        return stateHasChanged;
    }
    return false;
}

// This code runs when a close event is detected (SIGINT or CTRL_CLOSE_EVENT)
void programClose() {
    logSystem->print(Log::LOG_INFO, "User asked to close the program. Exiting...");
    logSystem->closeLogFile();
}


BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType)
{
    if ((int)dwCtrlType == CTRL_CLOSE_EVENT)
    {
        programClose();
        return TRUE;
    }
    return FALSE;
}

void startDisplay() {
    cout << "TouhouRPC - Discord Rich Presence status for Touhou games - Version " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_REVISION << endl;
    cout << "Available on GitHub: https://www.github.com/TheBakaRem/TouhouRPC" << endl;
    cout << "Copyright (C) 2020  Rem" << endl;
    cout << "Licensed under the GNU General Public License v3.0. This program comes with ABSOLUTELY NO WARRANTY; for details see the LICENSE file." << endl;
    cout << "This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License." << endl;
    cout << endl;

    cout << "Usage: Once started, the program will automatically attach to a Touhou game currently running on the computer." << endl;
    cout << "The program automatically detects when you change between supported games." << endl;
    cout << "You can close this program at any time by pressing (Ctrl+C)." << endl;
    cout << "Supported games: Touhou 06 (EoSD), 07 (PCB), 08 (IN), 09 (PoFV), 09.5 (StB), 10 (MoF), 11 (SA), 12 (UFO), 12.5 (DS), 12.8 (GFW), 13 (TD), 14 (DDC), 14.3 (ISC), 15 (LoLK), 16 (HSiFS), 16.5 (VD), 17 (WBaWC), 18 (UM)." << endl;
    cout << endl;
    
    cout << "!!THIS PROGRAM MIGHT BE ABLE TO TRIGGER ANTI-CHEAT SYSTEMS FROM OTHER GAMES, USE AT YOUR OWN RISK!!" << endl;
}

int main(int argc, char** argv)
{

    // Program start
    startDisplay();

    // SIGINT (Ctrl+C) detection
    std::signal(SIGINT, [](int) {
        programClose();
        std::exit(SIGINT);
        });

    // Console closing detection
    SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE);

    // PARSE CONFIG
    config->parseFile();


    // CONFIGURE LOG SYSTEM

    // Default values
    int activateLogFiles = 1; // log files activated by default
    int logLevelConsole, logLevelLogFile = Log::LOG_INFO; // LOG_INFO by default

    config->getValue("activateLogFiles", activateLogFiles);
    config->getValue("logLevelConsole", logLevelConsole);
    config->getValue("logLevelLogFile", logLevelLogFile);

    if (activateLogFiles) logSystem->openLogFile();

    logSystem->setLogLevelConsole(logLevelConsole);
    logSystem->setLogLevelLogFile(logLevelLogFile);

    // GET TOUHOU GAME
    std::unique_ptr<TouhouBase> touhouGame = getTouhouGame();

    // GET DISCORD LINK
    DiscordRPC discord = getDiscord(touhouGame->getClientId());
    if (!discord.isLaunched())
    {
        logSystem->print(Log::LOG_ERROR, "Discord is not running. Exiting program...");
        logSystem->closeLogFile();
        exit(-1);
    }
    
    logSystem->print(Log::LOG_DEBUG, "Starting Discord Rich Presence display...");
    
    // MAIN LOOP

    int const msBetweenTicks = 500;
    int const ticksPerSecond = 1000 / msBetweenTicks;
    int const ticksBetweenRegularUpdates = 16;
    int regularUpdateTickCount = ticksBetweenRegularUpdates;

    do {
        discord::Result res = discord.tickUpdate(msBetweenTicks);

        if (res != discord::Result::Ok)
        {
            DiscordRPC::showError(res, discordLastErrorType);
            discord = getDiscord(touhouGame->getClientId());
            
            if (touhouUpdate(touhouGame, discord) && touhouGame->isStillRunning())
            {
                discord.sendPresence(true);
            }
        }
        else {
            bool forceSend = regularUpdateTickCount >= ticksBetweenRegularUpdates;
            bool const touhouUpdated = touhouUpdate(touhouGame, discord);
            bool running = touhouGame->isStillRunning();
            if (running && (touhouUpdated || forceSend))
            {
                // if within 1 second of a normal update, make it a normal update instead of wasting an instant slot
                if (touhouUpdated && (ticksBetweenRegularUpdates - regularUpdateTickCount) <= ticksPerSecond)
                {
                    forceSend = true;
                }
                discord.sendPresence(forceSend);
                if (forceSend)
                {
                    regularUpdateTickCount = 0;
                }
            }
            regularUpdateTickCount++;
        }

        if (!touhouGame->isStillRunning()) {

            // Presence reset
            discord.resetPresence();
            discord.closeApp();

            logSystem->print(Log::LOG_INFO, "Game closed. Ready to find another supported game.");
            
            touhouGame = getTouhouGame();
            discord = getDiscord(touhouGame->getClientId());

            logSystem->print(Log::LOG_DEBUG, "Starting Discord Rich Presence display...");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(msBetweenTicks));
    } while (!interrupted);


    logSystem->print(Log::LOG_INFO, "Terminating program (reaching the end of the code).");
    logSystem->closeLogFile();
    return 0;
}

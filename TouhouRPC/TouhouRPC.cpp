// TouhouRPC.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

// Includes
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include "DiscordRPC.h"

#include "GameDetector.h"
#include "games/TouhouBase.h"

using namespace std;

namespace {
    volatile bool interrupted{ false };
}

DiscordRPC getDiscord(int64_t clientID) {
    DiscordRPC d = DiscordRPC(clientID);

    while (!d.isLaunched()) {
        std::cout << "Discord connection failed. Retrying in 5 seconds..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        d = DiscordRPC(clientID);
    }

    return d;
}

std::unique_ptr<TouhouBase> getTouhouGame() {
    std::unique_ptr<TouhouBase> d = initializeTouhouGame();

    while (d == nullptr) {
        std::cout << "No game found. Retrying in 5 seconds..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        d = initializeTouhouGame();
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

void startDisplay() {
    cout << "TOUHOU RPC - Discord Rich Presence status for Touhou games" << endl;
    cout << "Available on GitHub: https://www.github.com/FrDarky/TouhouRPC" << endl;
    cout << "Usage: Once started, the program will automatically attach to a Touhou game currently running on the computer." << endl;
    cout << "The program automatically detects when you change betwen supported games." << endl;
    cout << "You can close this program at any time by pressing (Ctrl+C)." << endl;
    cout << "Supported games: Touhou 06 (EoSD), 07 (PCB), 08 (IN), 11 (SA), 14 (DDC), 15 (LoLK), 16(HSiFS), 17 (WBaWC)." << endl;
    cout << endl;
    cout << "!!THIS PROGRAM MIGHT BE ABLE TO TRIGGER ANTI-CHEAT SYSTEMS FROM OTHER GAMES, USE AT YOUR OWN RISK!!" << endl;
    cout << endl;
}

int main()
{
    startDisplay();

    // SIGINT (Ctrl+C) detection
    std::signal(SIGINT, [](int) {
        std::cout << "Quitting program now..." << std::endl;
        std::exit(SIGINT);
        });


    // GET TOUHOU GAME
    std::unique_ptr<TouhouBase> touhouGame = getTouhouGame();

    // GET DISCORD LINK
    DiscordRPC discord = getDiscord(touhouGame->getClientId());
    if (!discord.isLaunched())
    {
        cout << "Discord is not started. Exiting..." << endl;
        exit(-1);
    }
    
    
    // MAIN LOOP

    int const msBetweenTicks = 500;
    int const ticksPerSecond = 1000 / msBetweenTicks;
    int const ticksBetweenRegularUpdates = 16;
    int regularUpdateTickCount = ticksBetweenRegularUpdates;

    do {
        discord::Result res = discord.tickUpdate(msBetweenTicks);

        if (res != discord::Result::Ok)
        {
            DiscordRPC::showError(res);
            discord = getDiscord(touhouGame->getClientId());
            
            if (touhouUpdate(touhouGame, discord))
            {
                discord.sendPresence(true);
            }
        }
        else {
            bool forceSend = regularUpdateTickCount >= ticksBetweenRegularUpdates;
            bool const touhouUpdated = touhouUpdate(touhouGame, discord);
            if (touhouUpdated || forceSend)
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
            discord.closeApp();

            std::cout << "Game closed. Trying to find another game to link to..." << endl << endl;
            touhouGame = getTouhouGame();

            discord = getDiscord(touhouGame->getClientId());
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(msBetweenTicks));
    } while (!interrupted);


    std::cout << "Terminating program..." << std::endl;

    return 0;
}

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

TouhouBase* getTouhouGame() {
    TouhouBase* d = initializeTouhouGame();

    while (d == nullptr) {
        std::cout << "No game found. Retrying in 5 seconds..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        d = initializeTouhouGame();
    }

    return d;
}

void touhouUpdate(TouhouBase* touhouGame, DiscordRPC & discord) {
    if (touhouGame->isStillRunning()) {

    // Update data in Touhou class
    touhouGame->readDataFromGameProcess();

    // Get data from Touhou class
    string gameName = "";
    string gameInfo = "";
    string largeImageIcon = "";
    string largeImageText = "";
    string smallImageIcon = "";
    string smallImageText = "";

    touhouGame->setGameName(gameName);
    touhouGame->setGameInfo(gameInfo);
    touhouGame->setLargeImageInfo(largeImageIcon, largeImageText);
    touhouGame->setSmallImageInfo(smallImageIcon, smallImageText);

    // Update RPC data
    discord.setActivityDetails(gameName, gameInfo, largeImageIcon, largeImageText, smallImageIcon, smallImageText);

    }
}

void startDisplay() {
    cout << "TOUHOU RPC - Discord Rich Presence status for Touhou games" << endl;
    cout << "Available on GitHub: https://www.github.com/FrDarky/TouhouRPC" << endl;
    cout << "Usage: Once started, the program will automatically attach to a Touhou game currently running on the computer." << endl;
    cout << "The program automatically detects when you change betwen supported games." << endl;
    cout << "You can close this program at any time by pressing (Ctrl+C)." << endl;
    cout << "Supported games: Touhou 06 (EoSD), 11 (SA), 15 (LoLK), 17 (WBaWC)." << endl;
    cout << endl;
    cout << "!!THIS PROGRAM MIGHT BE ABLE TO TRIGGER ANTI-CHEAT SYSTEMS FROM OTHER GAMES, USE AT YOUR OWN RISK!!" << endl;
    cout << endl;
}

int main()
{
    int64_t discordClientId = 684365704220508169;
    
    startDisplay();

    // SIGINT (Ctrl+C) detection
    std::signal(SIGINT, [](int) {
        std::cout << "Quitting program now..." << std::endl;
        std::exit(SIGINT);
        });

    
    // GET DISCORD LINK
    DiscordRPC discord = getDiscord(discordClientId);
    if (!discord.isLaunched()) {
        cout << "Discord is not started. Exiting..." << endl;
        exit(-1);
    }

    // GET TOUHOU GAME
    TouhouBase* touhouGame = getTouhouGame();

    
    
    // MAIN LOOP
    int count = 50;
    do {
        discord::Result res = discord.getCore()->RunCallbacks();
        
        if (res != discord::Result::Ok)
        {
            discord.showError(res);
            discord = getDiscord(discordClientId);
            
            if (touhouGame->isLinkedToProcess()) {
                touhouUpdate(touhouGame, discord);
            }

            discord.sendPresence();
        }
        else {
            if (count >= 50) {
                touhouUpdate(touhouGame, discord);
                discord.sendPresence();
                count -= 50;
            }
            count++;
        }

        if (!touhouGame->isStillRunning()) {

            // Presence reset
            discord.resetPresence();

            std::cout << "Game closed. Trying to find another game to link to..." << endl;
            touhouGame = getTouhouGame();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } while (!interrupted);


    std::cout << "Terminating program..." << std::endl;

    return 0;
}
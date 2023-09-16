#pragma once
#include <string>
#include <Windows.h>
#include <memoryapi.h>
#include <TlHelp32.h>
#include <handleapi.h>

import Games;

struct StateData {
    // States of play: for the most basic functionality, it is required that gameState must at least correctly switch between MainMenu and Playing.
    GameState gameState{ GameState::MainMenu };
    StageState stageState{ StageState::Stage };
    MainMenuState mainMenuState{ MainMenuState::TitleScreen };

    // Player details: all are required to be set.
    Character character{ Character::Reimu };
    SubCharacter subCharacter{ SubCharacter::None };
    Difficulty difficulty{ Difficulty::Easy };
    int lives{ 0 };
    int bombs{ 0 };
    int score{ 0 };
    int gameOvers{ 0 };

    // Photo details: Only used in photo-based games.
    int currentPhotoCount{ 0 };
    int requiredPhotoCount{ 0 };

    // ISC details : Main and sub items
    int mainItemUses{ 0 };
    int subItemUses{ 0 };

    // The following strictly used for data that we care about changing for the sake of immediate update (i.e. not score)
    void updateImportantData(StateData const& b);
    bool isImportantDataEqual(StateData const& b) const;
};

// Basic class that creates the relations between the game, TouhouRPC, and Discord's Rich Presence
class TouhouBase {
public:
    TouhouBase(PROCESSENTRY32W const& pe32);
    ~TouhouBase();

    // Check if the program is still linked to the game
    bool isLinkedToProcess() const { return linkedToProcess; };
    bool isStillRunning() const;

    // Represents the game on Discord's side
    virtual int64_t getClientId() const = 0;
    virtual const char* getGameName() const = 0;

    // Read data from the game
    virtual void readDataFromGameProcess() = 0;

    // Boss names must be implemented by each game, the rest are optional
    virtual std::string getMidbossName() const = 0;
    virtual std::string getBossName() const = 0;

    // e.g. "Stage 1" or "Extra Stage"
    virtual std::string getStageName() const;

    // Should just forward to correct tables in GameStrings, letting the game decide how to grab them
    virtual std::string getSpellCardName() const { return notSupported; } // spell card practice only
    virtual std::string getBGMName() const { return notSupported; } // music room only

    // Custom resources for games that need unique displays (photo games, LoLK pointdevice...)
    virtual std::string getCustomMenuResources() const { return notSupported; } // e.g. StB menu information. Adds details on the "info" line.
    virtual std::string getCustomResources() const { return notSupported; } // e.g. LoLK Pointdevice retry counts. Will only replace when in lives/bombs display mode.


    virtual bool stateHasChangedSinceLastCheck(); // Check if the state was updated between memory reads

    // Set data for Rich Presence
    virtual void setGameName(std::string& name) const;
    virtual void setGameInfo(std::string& info) const;
    virtual void setLargeImageInfo(std::string& icon, std::string& text) const;
    virtual void setSmallImageInfo(std::string& icon, std::string& text) const;

protected:
    HANDLE processHandle;
    StateData state;
    StateData prevState{ state };

    // We might migrate this in StateData
    int character{ 0 };
    int characterSub{ 0 };
    int stage{ 0 };
    int prevStage{ 0 };
    int difficulty{ 0 };
    int gameState{ 0 };
    int stageFrames{ 0 };
    int gameStateFrames{ 0 };
    int menuState{ 1 };

    // Functions only used internally for display.
    virtual std::string createFormattedScore() const;
    bool shouldShowCoverIcon() const;

    bool showScoreInsteadOfResources = false; // todo: make this a player driven option

    std::string const notSupported{ "not supported" }; // derived classes can return this when they need to return ref

private:
    bool linkedToProcess = false;
};
#pragma once
#include "TouhouBase.h"

// These enum classes are a union of possibilities for the games, they don't need to all be used by any 1 game.
enum class GameState
{
	MainMenu,
	Playing,
	StagePractice,
	SpellPractice,
	WatchingReplay,
	GameOver,
	Ending,
	StaffRoll,
};

enum class StageState
{
	Stage,
	Midboss,
	Boss,
};

enum class MainMenuState
{
	TitleScreen,
	GameStart,
	ExtraStart,
	StagePractice,
	SpellPractice,
	Replays,
	PlayerData,
	Achievements,
	MusicRoom,
	Options,
	Manual,
};

enum class Character
{
	// MCs
	Reimu,
	Marisa,

	// non-MCs
	Sakuya,
	Youmu,

	// IN Teams

	// IN Solo

};

enum class SubCharacter
{
	// Generic
	None,
	A,
	B,
	C,

	// DDC shot types have different images
	A_DistinctIcons,
	B_DistinctIcons,

	// SA partners

	// HSiFS seasons

	// WBaWC beasts
	Wolf,
	Otter,
	Eagle,
};

enum class Difficulty
{
	Easy,
	Normal,
	Hard,
	Lunatic,
	Extra,
	Phantasm,
};

// A class that allows for a consistent display of details for mainline Touhou games
class TouhouMainGameBase :
	public TouhouBase
{
public:
	TouhouMainGameBase(PROCESSENTRY32W const& pe32);

	// e.g. "Stage 1" or "Extra Stage"
	virtual std::string getStageName() const = 0;

	virtual std::string getMidbossName() const = 0;
	virtual std::string getBossName() const = 0;

	// Should just forward to correct tables in GameStrings, letting the game decide how to grab them
	virtual std::string const& getSpellCardName() const = 0; // spell card practice only
	virtual std::string const& getBGMName() const = 0; // music room only


	void setGameName(std::string& name) const override final;
	void setGameInfo(std::string& info) const override final;
	void setLargeImageInfo(std::string& icon, std::string& text) const override final;
	void setSmallImageInfo(std::string& icon, std::string& text) const override final;

private:
	bool shouldShowCoverIcon() const;
	std::string createFormattedScore() const;

protected:

	// Everything in this struct should get a value assigned to it.
	struct
	{
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
	} state;

	bool showScoreInsteadOfResources = false; // todo: make this a player driven option
	
	std::string const notSupported{ "not supported" }; // derived classes can return this when they need to return ref
};
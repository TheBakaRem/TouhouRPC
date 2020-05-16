#pragma once
#include "TouhouBase.h"

// These enum classes are a union of possibilities for the games, they don't need to all be used by any 1 game.
enum class GameState
{
	MainMenu,
	Stage,
	Midboss,
	Boss,
	SpellPractice,
	GameOver,
	Ending,
	StaffRoll,
};

enum class StageMode
{
	Standard,
	Practice,
	Replay,

	NotInStage,
};

enum class MainMenuState
{
	TitleScreen,
	GameStart,
	ExtraStart,
	PhantasmStart,
	StagePractice,
	SpellPractice,
	Replays,
	PlayerData,
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

	// SA partners

	// HSiFS seasons

	// WBaWC beasts

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

	// e.g. return "th14" for Touhou 14, used for finding assets
	virtual const char* getGameShortName() const = 0;

	// e.g. "Stage 1" or "Extra Stage"
	virtual std::string getStageName() const = 0;

	virtual std::string getMidbossName() const = 0;
	virtual std::string getBossName() const = 0;

	// Should just forward to correct tables in GameStrings, letting the game decide how to grab them
	virtual std::string const& getSpellCardName() const = 0;
	virtual std::string const& getBGMName() const = 0;


	void setGameName(std::string& name) const override final;
	void setGameInfo(std::string& info) const override final;
	void setLargeImageInfo(std::string& icon, std::string& text) const override final;
	void setSmallImageInfo(std::string& icon, std::string& text) const override final;

	bool isValidGameStateForStandardStageMode() const;
	std::string createFormattedScore() const;

protected:
	struct
	{
		GameState gameState{ GameState::MainMenu };
		StageMode stageMode{ StageMode::Standard };
		MainMenuState mainMenuState{ MainMenuState::TitleScreen };
		Character character{ Character::Reimu };
		SubCharacter subCharacter{ SubCharacter::A };
		Difficulty difficulty{ Difficulty::Easy };

		int lives{ 0 };
		int bombs{ 0 };
		int score{ 0 };
		int gameOvers{ 0 };
	} state;

	bool showScoreInsteadOfRes = false; // todo: make a player driven option
};
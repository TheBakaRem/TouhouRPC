#pragma once
#include "TouhouBase.h"

// These enum classes are a union of possibilities for the games, they don't need to all be used by any 1 game.
enum class GameState
{
	MainMenu,
	Playing,
	Playing_CustomResources, // alt resource support e.g. power for bombs
	StagePractice,
	SpellPractice,
	WatchingReplay,
	GameOver,
	Completed, // Used for scene-based games when succesfully completing a scene
	Fail, // Used for scene-based games when failing to complete a scene
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
	GameStart_Custom, // Used in scene-based games to display additional information on the profile like scenes completed and total score (ex: StB)
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
	Sanae,
	Youmu,
	Reisen,
	Cirno,
	Aya,

	// IN Teams
	Border,
	Magic,
	Scarlet,
	Nether,

	// IN Solo
	Yukari,
	Alice,
	Remilia,
	Yuyuko,

	// PoFV
	Lyrica,
	Merlin,
	Lunasa,
	Mystia,
	Tewi,
	Yuuka,
	Medicine,
	Komachi,
	Eiki,
};

enum class SubCharacter
{
	// Generic
	None,
	A,
	B,
	C,

	// IN
	Team,
	Solo,

	// PoFV
	AltColour,

	// DDC shot types have different images
	A_DistinctIcons,
	B_DistinctIcons,

	// SA partners
	AndYukari,
	AndSuika,
	AndAya,
	AndAlice,
	AndPatchouli,
	AndNitori,

	// HSiFS seasons
	Spring,
	Summer,
	Fall,
	Winter,

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
	Overdrive,
};

// A class that allows for a consistent display of details for mainline Touhou games
class TouhouMainGameBase :
	public TouhouBase
{
public:
	TouhouMainGameBase(PROCESSENTRY32W const& pe32);

	// Boss names must be implemented by game, the rest are optional
	virtual std::string getMidbossName() const = 0;
	virtual std::string getBossName() const = 0;

	// e.g. "Stage 1" or "Extra Stage"
	virtual std::string getStageName() const;

	// Should just forward to correct tables in GameStrings, letting the game decide how to grab them
	virtual std::string const& getSpellCardName() const { return notSupported; } // spell card practice only
	virtual std::string const& getBGMName() const { return notSupported; } // music room only

	virtual std::string getCustomMenuResources() const { return notSupported; } // e.g. StB menu information. Adds details on the "info" line.
	virtual std::string getCustomResources() const { return notSupported; } // e.g. LoLK Pointdevice retry counts. Will only replace when in lives/bombs display mode.

	bool stateHasChangedSinceLastCheck() override;

	void setGameName(std::string& name) const override;
	void setGameInfo(std::string& info) const override;
	void setLargeImageInfo(std::string& icon, std::string& text) const override;
	void setSmallImageInfo(std::string& icon, std::string& text) const override;

protected:
	std::string createFormattedScore() const;
	bool shouldShowCoverIcon() const;

	// Everything in this struct should get a value assigned to it.
	struct StateData
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

		// The following strictly used for data that we care about changing for the sake of immediate update (i.e. not score)
		void updateImportantData(StateData const& b);
		bool isImportantDataEqual(StateData const& b) const;
	} state;

	bool showScoreInsteadOfResources = false; // todo: make this a player driven option
	
	std::string const notSupported{ "not supported" }; // derived classes can return this when they need to return ref

private:
	StateData prevState{ state };
};
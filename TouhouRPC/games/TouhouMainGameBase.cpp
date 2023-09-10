#include "TouhouMainGameBase.h"

import Log;

TouhouMainGameBase::TouhouMainGameBase(PROCESSENTRY32W const& pe32)
	: TouhouBase(pe32)
{}

std::string TouhouMainGameBase::getStageName() const
{
	if (stage <= 6)
	{
		std::string name = "Stage ";
		name.append(std::to_string(stage));
		return name;
	}
	else
	{
		return "Extra Stage";
	}
}

void TouhouMainGameBase::StateData::updateImportantData(StateData const& b)
{
	gameState = b.gameState;
	stageState = b.stageState;
	gameOvers = b.gameOvers;

	lives = b.lives;
	bombs = b.bombs;

	currentPhotoCount = b.currentPhotoCount;
	requiredPhotoCount = b.requiredPhotoCount;

	mainItemUses = b.mainItemUses;
	subItemUses = b.subItemUses;
}

bool TouhouMainGameBase::StateData::isImportantDataEqual(StateData const& b) const
{
	return gameState == b.gameState
		&& stageState == b.stageState
		&& gameOvers == b.gameOvers

		&& lives == b.lives
		&& bombs == b.bombs

		&& currentPhotoCount == b.currentPhotoCount
		&& requiredPhotoCount == b.requiredPhotoCount

		&& mainItemUses == b.mainItemUses
		&& subItemUses == b.subItemUses
	;
}

bool TouhouMainGameBase::stateHasChangedSinceLastCheck()
{
	bool changed = TouhouBase::stateHasChangedSinceLastCheck() || !prevState.isImportantDataEqual(state);
	prevState.updateImportantData(state);
	return changed;
}

void TouhouMainGameBase::setGameName(std::string & name) const
{
	switch (state.gameState)
	{
	case GameState::MainMenu:
	{
		switch (state.mainMenuState)
		{
		case MainMenuState::TitleScreen: name = "On the title screen"; break;
		case MainMenuState::GameStart:
		case MainMenuState::GameStart_Custom: name = "Preparing to play"; break;
		case MainMenuState::ExtraStart: name = "Preparing to play Extra"; break;
		case MainMenuState::StagePractice: name = "Selecting a stage to practice"; break;
		case MainMenuState::SpellPractice: name = "Selecting a spell to practice"; break;
		case MainMenuState::Replays: name = "Selecting a replay"; break;
		case MainMenuState::PlayerData: name = "Viewing player data"; break;
		case MainMenuState::Achievements: name = "Viewing achievements"; break;
		case MainMenuState::AbilityCards: name = "Viewing ability cards"; break;
		case MainMenuState::MusicRoom: name = "In the music room:"; break; // game info will specify track.
		case MainMenuState::Options: name = "Changing options"; break;
		case MainMenuState::Manual: name = "Viewing the manual"; break;
		}
		break;
	}

	case GameState::GameOver:
	{
		name = ("Game over");
		break;
	}

	case GameState::Completed:
	{
		// Scene-based games: Completion of a scene
		name = getStageName();
		name.append(" completed!");
		break;
	}

	case GameState::Fail:
	{
		// Scene-based games: Failing completion of a scene
		name = getStageName();
		name.append(" failed...");
		break;
	}

	case GameState::Ending:
	case GameState::StaffRoll:
	{
		name = ("Cleared with ");
		name.append(createFormattedScore());
		name.append(" points");
		break;
	}

	case GameState::SpellPractice:
	{
		name = ("Practicing a spell:"); // game info will specify spell.
		break;
	}

	case GameState::WatchingReplay:
	{
		name = ("Watching a replay");
		break;
	}

	case GameState::StagePractice:
	{
		name = ("Practicing ");
		name.append(getStageName());
		break;
	}
	case GameState::Playing:
	{
		name.append(getStageName());

		// normal play shows resources or score
		name.reserve(name.size() + 6 /* text characters */ + 12 /* max score size */);
		name.append(" - (");
		if (showScoreInsteadOfResources)
		{
			name.append(createFormattedScore());
		}
		else
		{
			name.append(std::to_string(state.lives));
			name.append("/");
			name.append(std::to_string(state.bombs));
		}
		name.append(")");
		break;
	}
	case GameState::Playing_CustomResources:
	{
		name.append(getStageName());

		// normal play shows resources or score
		name.reserve(name.size() + 6 /* text characters */ + 12 /* max score size */);
		name.append(" - (");
		if (showScoreInsteadOfResources)
		{
			name.append(createFormattedScore());
		}
		else
		{
			name.append(getCustomResources());
		}
		name.append(")");
		break;
	}
	}
}

void TouhouMainGameBase::setGameInfo(std::string & info) const
{
	switch (state.gameState)
	{
	case GameState::MainMenu:
	{
		if (state.mainMenuState == MainMenuState::MusicRoom)
		{
			info = getBGMName();
		}
		if (state.mainMenuState == MainMenuState::GameStart_Custom)
		{
			info = getCustomMenuResources();
		}
		break;
	}

	case GameState::WatchingReplay:
	case GameState::Ending:
	case GameState::StaffRoll:
	case GameState::GameOver:
	case GameState::Fail:
	{
		break;
	}

	case GameState::Completed:
	{
		info = "Cleared with ";
		info.append(createFormattedScore());
		info.append(" points");
		break;
	}

	case GameState::SpellPractice:
	{
		info = getSpellCardName();
		break;
	}

	case GameState::Playing:
	case GameState::Playing_CustomResources:
	case GameState::StagePractice:
	{
		switch (state.stageState)
		{
		case StageState::Stage:
		{
			break;
		}
		case StageState::Midboss:
		{
			info = "Fighting ";
			info.append(getMidbossName());
			break;
		}

		case StageState::Boss:
		{
			info = "Fighting ";
			info.append(getBossName());
			break;
		}
		}
		break;
	}


	}
}

void TouhouMainGameBase::setLargeImageInfo(std::string & icon, std::string & text) const
{
	icon.clear(), text.clear();
	if (shouldShowCoverIcon())
	{
		icon.append("cover");
		return;
	}

	text = "Shot: ";

	switch (state.character)
	{
	case Character::Reimu:
	{
		icon.append("reimu"), text.append("Reimu");
		break;
	}
	case Character::Marisa:
	{
		icon.append("marisa"), text.append("Marisa");
		break;
	}
	case Character::Sakuya:
	{
		icon.append("sakuya"), text.append("Sakuya");
		break;
	}
	case Character::Sanae:
	{
		icon.append("sanae"), text.append("Sanae");
		break;
	}
	case Character::Youmu:
	{
		icon.append("youmu"), text.append("Youmu");
		break;
	}
	case Character::Reisen:
	{
		icon.append("reisen"), text.append("Reisen");
		break;
	}
	case Character::Cirno:
	{
		icon.append("cirno"), text.append("Cirno");
		break;
	}
	case Character::Aya:
	{
		icon.append("aya"), text.append("Aya");
		break;
	}

	// IN Teams
	case Character::Border:
	{
		icon.append("border"), text.append("Illusionary Barrier");
		break;
	}
	case Character::Magic:
	{
		icon.append("magic"), text.append("Aria of Forbidden Magic");
		break;
	}
	case Character::Scarlet:
	{
		icon.append("scarlet"), text.append("Visionary Scarlet Devil");
		break;
	}
	case Character::Nether:
	{
		icon.append("nether"), text.append("Netherworld Dwellers's");
		break;
	}
	
	// IN Solo
	case Character::Yukari:
	{
		icon.append("yukari"), text.append("Yukari");
		break;
	}
	case Character::Alice:
	{
		icon.append("alice"), text.append("Alice");
		break;
	}
	case Character::Remilia:
	{
		icon.append("remilia"), text.append("Remilia");
		break;
	}
	case Character::Yuyuko:
	{
		icon.append("yuyuko"), text.append("Yuyuko");
		break;
	}

	// PoFV
	case Character::Lyrica:
	{
		icon.append("lyrica"), text.append("Lyrica");
		break;
	}
	case Character::Merlin:
	{
		icon.append("merlin"), text.append("Merlin");
		break;
	}
	case Character::Lunasa:
	{
		icon.append("lunasa"), text.append("Lunasa");
		break;
	}
	case Character::Mystia:
	{
		icon.append("mystia"), text.append("Mystia");
		break;
	}
	case Character::Tewi:
	{
		icon.append("tewi"), text.append("Tewi");
		break;
	}
	case Character::Yuuka:
	{
		icon.append("yuuka"), text.append("Yuuka");
		break;
	}
	case Character::Medicine:
	{
		icon.append("medicine"), text.append("Medicine");
		break;
	}
	case Character::Komachi:
	{
		icon.append("komachi"), text.append("Komachi");
		break;
	}
	case Character::Eiki:
	{
		icon.append("eiki"), text.append("Eiki");
		break;
	}

	// ISC
	case Character::Seija:
	{
		icon.append("seija"), text.append("Seija");
		break;
	}
	}

	switch (state.subCharacter)
	{
	case SubCharacter::None:
	{
		break;
	}
	case SubCharacter::Team:
	{
		text.append(" Team");
		break;
	}
	case SubCharacter::Solo:
	{
		text.append(" Solo");
		break;
	}
	case SubCharacter::AltColour:
	{
		icon.append("alt");
		break;
	}
	case SubCharacter::A_DistinctIcons:
	{
		icon.append("a");
		// fall through
	}
	case SubCharacter::A:
	{
		text.append(" A");
		break;
	}
	case SubCharacter::B_DistinctIcons:
	{
		icon.append("b");
		// fall through
	}
	case SubCharacter::B:
	{
		text.append(" B");
		break;
	}
	case SubCharacter::C:
	{
		text.append(" C");
		break;
	}

	// SA partners
	case SubCharacter::AndYukari:
	{
		text.append(" + Yukari");
		break;
	}
	case SubCharacter::AndSuika:
	{
		text.append(" + Suika");
		break;
	}
	case SubCharacter::AndAya:
	{
		text.append(" + Aya");
		break;
	}
	case SubCharacter::AndAlice:
	{
		text.append(" + Alice");
		break;
	}
	case SubCharacter::AndPatchouli:
	{
		text.append(" + Patchouli");
		break;
	}
	case SubCharacter::AndNitori:
	{
		text.append(" + Nitori");
		break;
	}

	// HSiFS seasons
	case SubCharacter::Spring:
	{
		text.append(" (Spring)");
		break;
	}
	case SubCharacter::Summer:
	{
		text.append(" (Summer)");
		break;
	}
	case SubCharacter::Fall:
	{
		text.append(" (Fall)");
		break;
	}
	case SubCharacter::Winter:
	{
		text.append(" (Winter)");
		break;
	}

	// WBaWC beasts
	case SubCharacter::Wolf:
	{
		text.append(" (Wolf)");
		break;
	}
	case SubCharacter::Otter:
	{
		text.append(" (Otter)");
		break;
	}
	case SubCharacter::Eagle:
	{
		text.append(" (Eagle)");
		break;
	}
	}
}

void TouhouMainGameBase::setSmallImageInfo(std::string & icon, std::string & text) const
{
	icon.clear(), text.clear();
	if (shouldShowCoverIcon()) return;

	text = "Difficulty: ";
	switch (state.difficulty)
	{
	case Difficulty::NoDifficultySettings:
	{
		text.clear();
		break;
	}
	case Difficulty::Easy:
	{
		icon = "easy", text.append("Easy");
		break;
	}
	case Difficulty::Normal:
	{
		icon = "normal", text.append("Normal");
		break;
	}
	case Difficulty::Hard:
	{
		icon = "hard", text.append("Hard");
		break;
	}
	case Difficulty::Lunatic:
	{
		icon = "lunatic", text.append("Lunatic");
		break;
	}
	case Difficulty::Extra:
	{
		icon = "extra", text.append("Extra");
		break;
	}
	case Difficulty::Phantasm:
	{
		icon = "phantasm", text.append("Phantasm");
		break;
	}
	case Difficulty::Overdrive:
	{
		icon = "overdrive", text.append("Overdrive");
		break;
	}
	}
}

bool TouhouMainGameBase::shouldShowCoverIcon() const
{
	return state.gameState != GameState::Playing
		&& state.gameState != GameState::Playing_CustomResources
		&& state.gameState != GameState::Completed
		&& state.gameState != GameState::Fail
		&& state.gameState != GameState::StagePractice
		&& state.gameState != GameState::SpellPractice
		&& state.gameState != GameState::WatchingReplay
		;
}

std::string TouhouMainGameBase::createFormattedScore() const
{
	std::string scoreString = std::to_string((state.score * 10) + state.gameOvers);
	int insertPosition = scoreString.length() - 3; // Do NOT use size_t as it is unsigned and can't be properly tested in the loop after, causing std::out_of_range exceptions.
	while (insertPosition > 0)
	{
		scoreString.insert(insertPosition, ",");
		insertPosition -= 3;
	}
	return scoreString;
}
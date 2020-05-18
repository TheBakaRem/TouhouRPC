#include "TouhouMainGameBase.h"

TouhouMainGameBase::TouhouMainGameBase(PROCESSENTRY32W const& pe32)
	: TouhouBase(pe32)
{}

void TouhouMainGameBase::setGameName(std::string & name) const
{
	switch (state.gameState)
	{
	case GameState::MainMenu:
	{
		switch (state.mainMenuState)
		{
		case MainMenuState::TitleScreen: name = "On the title screen"; break;
		case MainMenuState::GameStart: name = "Preparing to play"; break;
		case MainMenuState::ExtraStart: name = "Preparing to play Extra"; break;
		case MainMenuState::StagePractice: name = "Selecting a stage to practice"; break;
		case MainMenuState::SpellPractice: name = "Selecting a spell to practice"; break;
		case MainMenuState::Replays: name = "Selecting a replay"; break;
		case MainMenuState::PlayerData: name = "Viewing player data"; break;
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

	case GameState::Ending:
	case GameState::StaffRoll:
	{
		name = ("Cleared with ");
		name.append(createFormattedScore());
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
		break;
	}

	case GameState::WatchingReplay:
	case GameState::Ending:
	case GameState::StaffRoll:
	case GameState::GameOver:
	{
		break;
	}

	case GameState::SpellPractice:
	{
		info = getSpellCardName();
		break;
	}

	case GameState::Playing:
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
	}
	switch (state.subCharacter)
	{
	case SubCharacter::None:
	{
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
	}
}

void TouhouMainGameBase::setSmallImageInfo(std::string & icon, std::string & text) const
{
	icon.clear(), text.clear();
	if (shouldShowCoverIcon()) return;

	text = "Difficulty: ";
	switch (state.difficulty)
	{
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
	}
}

bool TouhouMainGameBase::shouldShowCoverIcon() const
{
	return state.gameState != GameState::Playing
		&& state.gameState != GameState::StagePractice
		&& state.gameState != GameState::SpellPractice
		&& state.gameState != GameState::WatchingReplay
		;
}

std::string TouhouMainGameBase::createFormattedScore() const
{
	std::string scoreString = std::to_string((state.score * 10) + state.gameOvers);
	size_t insertPosition = scoreString.length() - 3;
	while (insertPosition > 0)
	{
		scoreString.insert(insertPosition, ",");
		insertPosition -= 3;
	}
	return scoreString;
}
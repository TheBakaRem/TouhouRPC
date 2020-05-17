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
		case MainMenuState::PhantasmStart: name = "Preparing to play Phantasm"; break;
		case MainMenuState::StagePractice: name = "Selecting a stage to practice"; break;
		case MainMenuState::SpellPractice: name = "Selecting a spell to practice"; break;
		case MainMenuState::Replays: name = "Selecting a replay"; break;
		case MainMenuState::PlayerData: name = "Viewing player data"; break;
		case MainMenuState::MusicRoom: name = "In the music room:"; break; // game info will specify track.
		case MainMenuState::Options: name = "Changing options"; break;
		case MainMenuState::Manual: name = "Viewing the manual"; break;
		}
		return;
	}

	case GameState::GameOver:
	{
		name = ("Game over");
		return;
	}

	case GameState::Ending:
	case GameState::StaffRoll:
	{
		name = ("Cleared with ");
		name.append(createFormattedScore());
		return;
	}

	case GameState::SpellPractice:
	{
		name = ("Practicing a spell:"); // game info will specify spell.
		return;
	}

	case GameState::Stage:
	case GameState::Midboss:
	case GameState::Boss:
	{
		if (state.stageMode == StageMode::Replay)
		{
			name = ("Watching a replay");
			return;
		}

		if (state.stageMode == StageMode::Practice)
		{
			name = ("Practicing ");
		}

		// Normal play
		name.append(getStageName());

		if (state.stageMode == StageMode::Standard)
		{
			name.append(" - (");
			if (showScoreInsteadOfRes)
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
		}
		break;
	}
	}
}

void TouhouMainGameBase::setGameInfo(std::string & info) const
{
	if (state.stageMode == StageMode::Replay)
	{
		return;
	}

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

	case GameState::Stage:
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

	case GameState::Midboss: // Mid-bosses
	{
		info = "Fighting ";
		info.append(getMidbossName());
		break;
	}

	case GameState::Boss: // Bosses
	{
		info = "Fighting ";
		info.append(getBossName());
		break;
	}

	}
}

void TouhouMainGameBase::setLargeImageInfo(std::string & icon, std::string & text) const
{
	icon = getGameShortName(), text.clear();
	if (state.stageMode == StageMode::NotInStage)
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
	if (state.stageMode == StageMode::NotInStage) return;

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

bool TouhouMainGameBase::isValidGameStateForStandardStageMode() const
{
	return state.gameState == GameState::Stage || state.gameState == GameState::Midboss || state.gameState == GameState::Boss || state.gameState == GameState::SpellPractice;
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
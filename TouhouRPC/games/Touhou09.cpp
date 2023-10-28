import "Touhou09.h";


Touhou09::Touhou09(PROCESSENTRY32W const& pe32) : TouhouBase(pe32) {}

Touhou09::~Touhou09() {}

void Touhou09::readDataFromGameProcess() {
    gameMode = GameModePoFV::Normal;
    state.gameState = GameState::Playing;
    state.stageState = StageState::Stage;
    state.mainMenuState = MainMenuState::TitleScreen;

    // MENUS
    menuState = ReadProcessMemoryInt(processHandle, MENU_MODE, 1);
    int inMenu = ReadProcessMemoryInt(processHandle, IN_MENU, 1);
    if (inMenu == 0 || inMenu == 14) {
        state.gameState = GameState::MainMenu;
        switch (menuState) {
            default:
            case 176: state.mainMenuState = MainMenuState::TitleScreen; break;
            case 196:
            case 180: state.mainMenuState = MainMenuState::GameStart; break;
            case 40: state.mainMenuState = MainMenuState::Replays; break;
            case 164:
            {
                state.mainMenuState = MainMenuState::MusicRoom;

                TouhouAddress musicRoomPtr = ReadProcessMemoryInt(processHandle, MUSIC_ROOM_POINTER);
                if (musicRoomPtr) {
                    bgm = ReadProcessMemoryInt(processHandle, (musicRoomPtr + MUSIC_ROOM_SELECTION_OFFSET));
                }
                break;
            }
        }
    }
    else {
        switch (menuState) {
            case 180: gameMode = GameModePoFV::Match; // fallthrough
            default:
            case 196: state.gameState = GameState::Playing; break;
            case 40: state.gameState = GameState::WatchingReplay; break;
        }
    }

    // GENERAL
    difficulty = ReadProcessMemoryInt(processHandle, DIFFICULTY);
    stage = ReadProcessMemoryInt(processHandle, STAGE);
    location = ReadProcessMemoryInt(processHandle, LOCATION);
    roundNum = ReadProcessMemoryInt(processHandle, ROUND_NUMBER);

    int matchModeMode = ReadProcessMemoryInt(processHandle, MATCH_MODE_MODE);
    if (gameMode == GameModePoFV::Match && matchModeMode == 2) {
        gameMode = GameModePoFV::MatchAsP2;
    }

    // PLAYER 1

    p1.chara = ReadProcessMemoryInt(processHandle, P1_CHARACTER);
    p1.altCol = ReadProcessMemoryInt(processHandle, P1_ALT_COLOUR);
    p1.wins = ReadProcessMemoryInt(processHandle, P1_WIN_COUNT);

    TouhouAddress p1StatePtr = ReadProcessMemoryInt(processHandle, P1_STATE_POINTER);
    if (p1StatePtr) {
        int attempts = ReadProcessMemoryInt(processHandle, (p1StatePtr + P1_ATTEMPTS_REMAINING), 2);
        switch (attempts) {
            default:
            case 0: p1.attempts = 0; break;
            case 16256: p1.attempts = 1; break;
            case 16384: p1.attempts = 2; break;
            case 16448: p1.attempts = 3; break;
            case 16512: p1.attempts = 4; break;
        }

        p1.score = ReadProcessMemoryInt(processHandle, (p1StatePtr + P1_SCORE_OFFSET));
        p1.gameOvers = ReadProcessMemoryInt(processHandle, (p1StatePtr + P1_GAMEOVERS_OFFSET));
    }

    TouhouAddress p1LivesPtr = ReadProcessMemoryInt(processHandle, P1_LIVES_POINTER);
    if (p1LivesPtr) {
        p1.lives = ReadProcessMemoryInt(processHandle, (p1StatePtr + P1_LIVES_OFFSET));
    }

    // PLAYER 2
    p2.chara = ReadProcessMemoryInt(processHandle, P2_CHARACTER);
    p2.altCol = ReadProcessMemoryInt(processHandle, P2_ALT_COLOUR);
    p2.wins = ReadProcessMemoryInt(processHandle, P2_WIN_COUNT);

    TouhouAddress p2StatePtr = ReadProcessMemoryInt(processHandle, P2_STATE_POINTER);
    if (p2StatePtr) {
        p2.score = ReadProcessMemoryInt(processHandle, (p2StatePtr + P2_SCORE_OFFSET));
        p2.gameOvers = ReadProcessMemoryInt(processHandle, (p2StatePtr + P2_GAMEOVERS_OFFSET));
    }

    TouhouAddress p2LivesPtr = ReadProcessMemoryInt(processHandle, P2_LIVES_POINTER);
    if (p2LivesPtr) {
        p2.lives = ReadProcessMemoryInt(processHandle, (p2StatePtr + P2_LIVES_OFFSET));
    }

    bool const useP1 = (gameMode != GameModePoFV::MatchAsP2);

    switch (useP1 ? p1.chara : p2.chara) {
        default:
        case 0: state.character = Character::Reimu; break;
        case 1: state.character = Character::Marisa; break;
        case 2: state.character = Character::Sakuya; break;
        case 3: state.character = Character::Youmu; break;
        case 4: state.character = Character::Reisen; break;
        case 5: state.character = Character::Cirno; break;
        case 6: state.character = Character::Lyrica; break;
        case 7: state.character = Character::Mystia; break;
        case 8: state.character = Character::Tewi; break;
        case 9: state.character = Character::Yuuka; break;
        case 10: state.character = Character::Aya; break;
        case 11: state.character = Character::Medicine; break;
        case 12: state.character = Character::Komachi; break;
        case 13: state.character = Character::Eiki; break;
        case 14: state.character = Character::Merlin; break;
        case 15: state.character = Character::Lunasa; break;
    }

    state.subCharacter = (useP1 ? p1.altCol == 1 : p2.altCol == 1) ? SubCharacter::AltColour : SubCharacter::None;
    state.score = (useP1 ? p1.score : p2.score);
    state.gameOvers = (useP1 ? p1.gameOvers : p2.gameOvers);

    if (gameMode == GameModePoFV::Normal) {
        state.lives = p1.attempts;
    }

    switch (difficulty) {
        default:
        case 0: state.difficulty = Difficulty::Easy; break;
        case 1: state.difficulty = Difficulty::Normal; break;
        case 2: state.difficulty = Difficulty::Hard; break;
        case 3: state.difficulty = Difficulty::Lunatic; break;
        case 4: state.difficulty = Difficulty::Extra; break;
    }
}

void Touhou09::setGameName(std::string& name) const {
    switch (state.gameState) {
        default: TouhouBase::setGameName(name); break;

        case GameState::Playing:
        {
            switch (gameMode) {
                case GameModePoFV::Normal:
                {
                    name.append(getStageName());

                    if (state.difficulty != Difficulty::Extra) {
                        // normal play shows resources or score
                        name.append(" - (");
                        if (showScoreInsteadOfResources) {
                            name.append(createFormattedScore());
                        }
                        else {
                            name.append(std::to_string(p1.attempts));
                            name.append(" chances");
                        }
                        name.append(")");
                    }
                    break;
                }
                case GameModePoFV::Match:
                case GameModePoFV::MatchAsP2:
                {
                    name.append(getStageName());
                    break;
                }
            }
            break;
        }
    }
}

void Touhou09::setGameInfo(std::string& info) const {
    switch (state.gameState) {
        default: TouhouBase::setGameInfo(info); break;

        case GameState::Playing:
        {
            switch (gameMode) {
                case GameModePoFV::Normal:
                {
                    info.append(getCharacterName(p1.chara));
                    if (state.difficulty != Difficulty::Extra) {
                        info.append(" (");
                        std::string p1Lives = std::to_string(static_cast<double>(p1.lives) / 2.0);
                        p1Lives.resize(3); // lazy way to get 1 d.p.
                        info.append(p1Lives);
                        info.append(")");
                    }

                    info.append(" vs ");
                    info.append(getCharacterName(p2.chara));

                    if (state.difficulty != Difficulty::Extra) {
                        info.append(" (");
                        std::string p2Lives = std::to_string(static_cast<double>(p2.lives) / 2.0);
                        p2Lives.resize(3);
                        info.append(p2Lives);
                        info.append(")");
                    }
                    break;
                }
                case GameModePoFV::Match:
                case GameModePoFV::MatchAsP2:
                {
                    info.append(getCharacterName(p1.chara));
                    info.append(" (");
                    info.append(std::to_string(p1.wins));
                    info.append(") vs ");
                    info.append(getCharacterName(p2.chara));
                    info.append(" (");
                    info.append(std::to_string(p2.wins));
                    info.append(")");
                    break;
                }
            }
            break;
        }

    }
}

std::string Touhou09::getStageName() const {
    if (gameMode == GameModePoFV::Normal) {
        std::string ret = (difficulty == 4) ? "Extra Stage " : "Stage ";
        ret.append(std::to_string(stage + 1));
        return ret;
    }
    else {
        std::string ret = "Round ";
        ret.append(std::to_string(roundNum + 1));
        if (location < 14) {
            // location can be set to 255 before setting itself correctly
            ret.append(" - ");
            ret.append(th09_stageNames[location]);
        }
        return ret;
    }
}

std::string Touhou09::getBGMName() const {
    return th09_musicNames[bgm];
}

std::string Touhou09::getCharacterName(int chara) const {
    return th09_characterNames[chara];
}

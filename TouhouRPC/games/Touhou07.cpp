import "Touhou07.h";


Touhou07::Touhou07(PROCESSENTRY32W const& pe32) : TouhouBase(pe32) {}

Touhou07::~Touhou07() {}

void Touhou07::readDataFromGameProcess() {
    // TODO
    // need a way to distinguish options menu from title screen
    // need a way to distinguish replay select and extra start submenu from start submenu
    // need a way to know if we've game over'd, are watching the credits, or watching the staff roll
    // since these are minor and PCB's data is laid out in a pretty hellish way, tempted to just leave it

    // Reset mainMenuState, bgm will tell us if we're in the menu
    int mainMenuState = -1;
    state.gameState = GameState::Playing;
    state.stageState = StageState::Stage;

    // this is 0 only if we're not playing currently
    int inGameFlagB = ReadProcessMemoryInt(processHandle, IN_GAME_FLAG_B);
    int stageMode = ReadProcessMemoryInt(processHandle, STAGE_MODE, 1);

    if (inGameFlagB == 0 || (stageMode & STAGE_MODE_DEMO_FLAG) != 0) {
        state.gameState = GameState::MainMenu;

        int menu_pointer = ReadProcessMemoryInt(processHandle, MENU_POINTER);
        if (menu_pointer) {
            // this menu state seems to be all over the place. might be tied to background image being displayed
            mainMenuState = ReadProcessMemoryInt(processHandle, (menu_pointer + 0x0C));

            switch (mainMenuState) {
                default:
                case 130: state.mainMenuState = MainMenuState::TitleScreen; break;
                case 47: state.mainMenuState = MainMenuState::PlayerData; break;
                case 35: state.mainMenuState = MainMenuState::MusicRoom; break;

                case 129:
                {
                    // 129 is anything other than the above special cases. we can distinguish them a little. todo: figure out how to distinguish the rest
                    if ((stageMode & STAGE_MODE_PRACTICE_FLAG) != 0) {
                        state.mainMenuState = MainMenuState::StagePractice;
                    }
                    else {
                        state.mainMenuState = MainMenuState::GameStart;
                    }
                    break;
                }
            }
        }

        if (state.mainMenuState == MainMenuState::MusicRoom) {
            bgm = ReadProcessMemoryInt(processHandle, MUSIC_ROOM_TRACK);
        }
    }

    stage = ReadProcessMemoryInt(processHandle, STAGE);
    if (state.gameState == GameState::Playing) {
        if ((stageMode & STAGE_MODE_PRACTICE_FLAG) != 0) {
            state.gameState = GameState::StagePractice;
        }
        else if ((stageMode & STAGE_MODE_REPLAY_FLAG) != 0) {
            state.gameState = GameState::WatchingReplay;
        }

        int bossFlag = ReadProcessMemoryInt(processHandle, BOSS_FLAG);
        if (bossFlag == 1) {
            // fighting either a boss or a midboss, check which it is
            int midBossFlag = ReadProcessMemoryInt(processHandle, IS_MAIN_BOSS, 1);
            state.stageState = (midBossFlag == 3) ? StageState::Boss : StageState::Midboss;
            if (stage == 6) {
                // For some reason stage 6 youmu also has flag == 3 so let's take a guess based on frame counter
                // from testing, 5200 seems a reasonable cutoff
                int frameCounter = ReadProcessMemoryInt(processHandle, FRAME_COUNTER);
                state.stageState = (frameCounter < 5200) ? StageState::Midboss : StageState::Boss;
            }
        }
    }

    // Read character and difficulty info
    int chara = ReadProcessMemoryInt(processHandle, CHARACTER, 1);
    switch (chara) {
        default:
        case 0: state.character = Character::Reimu; break;
        case 1: state.character = Character::Marisa; break;
        case 2: state.character = Character::Sakuya; break;
    }

    int charaSub = ReadProcessMemoryInt(processHandle, SUB_CHARACTER, 1);
    switch (charaSub) {
        default:
        case 0: state.subCharacter = SubCharacter::A; break;
        case 1: state.subCharacter = SubCharacter::B; break;
    }

    difficulty = ReadProcessMemoryInt(processHandle, DIFFICULTY);
    switch (difficulty) {
        default:
        case 0: state.difficulty = Difficulty::Easy; break;
        case 1: state.difficulty = Difficulty::Normal; break;
        case 2: state.difficulty = Difficulty::Hard; break;
        case 3: state.difficulty = Difficulty::Lunatic; break;
        case 4: state.difficulty = Difficulty::Extra; break;
        case 5: state.difficulty = Difficulty::Phantasm; break;
    }

    // Read current game progress
    int player_pointer = ReadProcessMemoryInt(processHandle, PLAYER_POINTER);
    if (player_pointer) {
        // Behold zungramming: Lives and bombs are actually floats in memory.
        state.lives = static_cast<int>(ReadProcessMemoryFloat(processHandle, (player_pointer + 0x5C)));
        state.bombs = static_cast<int>(ReadProcessMemoryFloat(processHandle, (player_pointer + 0x68)));
        state.score = ReadProcessMemoryInt(processHandle, player_pointer);
        state.gameOvers = ReadProcessMemoryInt(processHandle, (player_pointer + 0x20), 1);
    }
}

std::string Touhou07::getStageName() const {
    if (stage <= 7) {
        return TouhouBase::getStageName();
    }
    else {
        return "Phantasm Stage";
    }
}

std::string Touhou07::getMidbossName() const {
    switch (stage) {
        case 1: return "Cirno";
        case 2: return "Chen";
        case 3: return "Alice Margatroid";
        case 4: return "Lily White";
        case 5:
        case 6: return "Youmu Konpaku";
        case 7: return "Chen";
        case 8: return "Ran Yakumo";
        default: return "";
    }
}

std::string Touhou07::getBossName() const {
    switch (stage) {
        case 1: return "Letty Whiterock";
        case 2: return "Chen";
        case 3: return "Alice Margatroid";
        case 4: return "Prismriver Sisters";
        case 5: return "Youmu Konpaku";
        case 6: return "Yuyuko Saigyouji";
        case 7: return "Ran Yakumo";
        case 8: return "Yukari Yakumo";
        default: return "";
    }
}

std::string Touhou07::getBGMName() const {
    return th07_musicNames[bgm];
}

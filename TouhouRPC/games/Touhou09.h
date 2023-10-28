#pragma once
import Games;
import WindowsUtils;


enum class GameModePoFV {
    Normal,
    Match,
    MatchAsP2, // Shikigami v Human
};

class Touhou09 : public TouhouBase {
public:
    Touhou09(PROCESSENTRY32W const& pe32);
    ~Touhou09();

    // Inherited from TouhouBase
    int64_t getClientId() const override { return 717460728990139023; };
    const char* getGameName() const override { return "Touhou 09 - Phantasmagoria of Flower View"; }

    void readDataFromGameProcess() override;

    void setGameName(std::string& name) const override;
    void setGameInfo(std::string& info) const override;

    // Inherited from TouhouBase
    std::string getMidbossName() const override { return notSupported; }
    std::string getBossName() const override { return notSupported; }
    std::string getStageName() const override;
    std::string getBGMName() const override;

    std::string getCharacterName(int character) const;

protected:
    GameModePoFV gameMode{ GameModePoFV::Normal };
    unsigned int bgm{ 0 }; // correct in music room only
    unsigned int location{ 0 };
    unsigned int roundNum{ 0 };

    struct PlayerData {
        int attempts{ 0 };

        int chara{ 0 };
        int altCol{ 0 };
        int score{ 0 };
        int gameOvers{ 0 };
        int lives{ 0 };
        int wins{ 0 };
    };

    PlayerData p1;
    PlayerData p2;

private:
    // addresses correct for v1.50a
    enum address : TouhouAddress {
        // Menus
        MENU_MODE = 0x004A6A5CL, // 1 byte
        IN_MENU = 0x004A7EC4L, // 1 byte, 0/14 on menu/demo, 4 in game, 12 in replay
        MUSIC_ROOM_POINTER = 0x004AC8C0L, // offset 0x11B74 is music room selection
        MUSIC_ROOM_SELECTION_OFFSET = 0x011B74,

        // General
        DIFFICULTY = 0x004A7EACL,
        STAGE = 0x004A7E8CL,
        LOCATION = 0x004DC690L, // can also be 255 while match begins.
        ROUND_NUMBER = 0x004A7E90L,
        MATCH_MODE_MODE = 0x004A7EA4L, // if == 2, we're in shikigami v human mode and should read from p2 data

        // Player 1
        P1_CHARACTER = 0x004A7DB0L,
        P1_ALT_COLOUR = 0x004A7DC0L,
        P1_STATE_POINTER = 0x004A7DACL,
        P1_ATTEMPTS_REMAINING = 0x02, // 2 bytes
        P1_SCORE_OFFSET = 0x08,
        P1_GAMEOVERS_OFFSET = 0x14,
        P1_LIVES_POINTER = 0x004A7D94L,
        P1_LIVES_OFFSET = 0xA8,
        P1_WIN_COUNT = 0x004A7E98L,

        // Player 2
        P2_CHARACTER = 0x004A7DE8L,
        P2_ALT_COLOUR = 0x004A7DF8L,
        P2_STATE_POINTER = 0x004A7DE4L,
        P2_SCORE_OFFSET = P1_SCORE_OFFSET,
        P2_GAMEOVERS_OFFSET = P1_GAMEOVERS_OFFSET,
        P2_LIVES_POINTER = 0x004A7DCCL,
        P2_LIVES_OFFSET = P1_LIVES_OFFSET,
        P2_WIN_COUNT = 0x004A7E9CL,
    };
};

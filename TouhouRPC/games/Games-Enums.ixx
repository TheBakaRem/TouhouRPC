export module Games:Enums;

// These enum classes are a union of possibilities for the games, they don't need to all be used by any 1 game.
export enum class GameState {
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

export enum class StageState {
    Stage,
    Midboss,
    Boss,
};

export enum class MainMenuState {
    TitleScreen,
    GameStart,
    GameStart_Custom, // Used in scene-based games to display additional information on the profile like scenes completed and total score (e.g. StB)
    ExtraStart,
    StagePractice,
    SpellPractice,
    Replays,
    PlayerData,
    Achievements,
    AbilityCards,
    MusicRoom,
    Options,
    Manual,
};

export enum class Character {
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

    // DS
    Hatate,

    // ISC
    Seija,
};

export enum class SubCharacter {
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

export enum class Difficulty {
    NoDifficultySettings = -1, // Used for games without difficulty settings (e.g. StB)
    Easy = 0,
    Normal,
    Hard,
    Lunatic,
    Extra,
    Phantasm,
    Overdrive,
};

// Because TH13+ uses a GameMode value.
// TODO: Remove this stuff and make it cleaner or use it for all games instead of modern era only.
export enum GameMode {
    GAME_MODE_STANDARD = 1, // main menu, endings, staff roll, and normal gameplay
    GAME_MODE_REPLAY = 2,
    GAME_MODE_CLEAR = 3, // seems to appear after staff roll ends, but goes back to 1 on the name registration screen.
    GAME_MODE_PRACTICE = 4,
    GAME_MODE_SPELLPRACTICE = 5,
};
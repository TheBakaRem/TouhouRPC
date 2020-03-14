#include <iostream>
#include "Touhou08.h"

Touhou08::Touhou08(PROCESSENTRY32* pe32) : TouhouBase(pe32)
{
	isBoss = 0;
    bossStateChange = 0;
    oldStageFrames = 0;
	spellCardID = 0;
}

Touhou08::~Touhou08()
{
}

void Touhou08::readDataFromGameProcess()
{
    bool oldIsBoss;
    int oldStage; // to reset bossStateChange

    // PLAYER
    ReadProcessMemory(processHandle, (LPCVOID)CHARACTER, (LPVOID)&character, 1, NULL);

    // DIFFICULTY
    ReadProcessMemory(processHandle, (LPCVOID)DIFFICULTY, (LPVOID)&difficulty, 1, NULL);

    // FRAMES
    oldStageFrames = stageFrames;
    ReadProcessMemory(processHandle, (LPCVOID)STAGE_FRAMES, (LPVOID)&stageFrames, 4, NULL);

    // STAGE
    oldStage = stage;
    if (oldStageFrames - stageFrames > 0)
    {
        oldStage = -1;
    }
    ReadProcessMemory(processHandle, (LPCVOID)STAGE, (LPVOID)&stage, 1, NULL);
    if (stage != oldStage)
    {
        bossStateChange = 0;
    }

    // BOSS
    oldIsBoss = isBoss;
    ReadProcessMemory(processHandle, (LPCVOID)BOSS_APPEARANCE, (LPVOID)&isBoss, 1, NULL);
    if (oldIsBoss != isBoss)
    {
        bossStateChange++;
    }

    // SPELL_CARD_ID
    ReadProcessMemory(processHandle, (LPCVOID)SPELL_CARD_ID, (LPVOID)&spellCardID, 1, NULL);
}

void Touhou08::setGameName(std::string& name)
{
    name.assign("Touhou 8 - Imperishable Night");
}

void Touhou08::setGameInfo(std::string& info)
{
    if (stageFrames - oldStageFrames == 0)
    {
        info.assign("In the menu"); // main menu, pause menu, etc
        return;
    }

    std::string BGM;
    ReadProcessMemory(processHandle, (LPCVOID)PLAYING_MUSIC, (LPVOID)&BGM, 1, NULL);
    if (BGM[0] != 'b')
    {
        // spellcard practice
        info.assign("Practicing a spell\n");
        info.append(th08_spellCardName[spellCardID]);
        return;
    }

    info.assign(stageName[stage]);

    // boss
    if (isBoss)
    {
        info.append(" | Fighting ");
        // screw stage 5, 6B, ex
        if (stage == 5 || stage == 7 || stage == 8)
        {
            switch (bossStateChange)
            {
            case 0: // for people who open in middle of a replay
            case 1: // but really launch this before launching your game
                info.append(midBossName[stage]);
                break;
            default: 
                info.append(bossName[stage]);
            }
        }
        else
        {
            info.append(bossName[stage]);
        }
    }
}

void Touhou08::setLargeImageInfo(std::string& icon, std::string& text)
{
    icon.assign("");
    text.assign("");

    if (stageFrames - oldStageFrames == 0) return; // So that the rest isn't executed while in the menus

    std::string charTeamName;
    switch (character)
    {
    case 0: // border team:
        charTeamName = "Border Team";
        icon.assign("th08borderteam");
        break;
    case 1: // magic team
        charTeamName = "Magic Team";
        icon.assign("th08magicteam");
        break;
    case 2: // scarlet team
        charTeamName = "Scarlet Team";
        icon.assign("th08scarletteam");
        break;
    case 3: // ghost team
        charTeamName = "Ghost Team";
        icon.assign("th08ghostteam");
        break;
    case 4: // reimu
        charTeamName = "Reimu";
        icon.assign("th08reimu");
        break;
    case 5: // yukari
        charTeamName = "Yukari";
        icon.assign("th08yukari");
        break;
    case 6: // marisa
        charTeamName = "Marisa";
        icon.assign("th08marisa");
        break;
    case 7: // alice
        charTeamName = "Alice";
        icon.assign("th08alice");
        break;
    case 8: // sakuya
        charTeamName = "Sakuya";
        icon.assign("th08sakuya");
        break;
    case 9: // remilia
        charTeamName = "Remilia";
        icon.assign("th08remilia");
        break;
    case 10: // youmu
        charTeamName = "Youmu";
        icon.assign("th08youmu");
        break;
    case 11: // yuyuko
        charTeamName = "Yuyuko";
        icon.assign("th08yuyuko");
        break;
    }
    text.assign(charTeamName);
}

void Touhou08::setSmallImageInfo(std::string& icon, std::string& text)
{
    icon.assign("");
    text.assign("");
    if (stageFrames - oldStageFrames == 0) return; // So that the rest isn't executed while in the menus

    text.append("Difficulty: ");
    switch (difficulty) {
    case 0: // Easy
        text.append("Easy");
        icon.assign("easy");
        break;
    case 1: // Normal or Last Word
        text.append("Normal");
        icon.assign("normal");
        break;
    case 2: // Hard
        text.append("Hard");
        icon.assign("hard");
        break;
    case 3: // Lunatic
        text.append("Lunatic");
        icon.assign("lunatic");
        break;
    case 4: // Extra (difficulty == 4)
        text.append("Extra");
        icon.assign("extra");
        break;
    }
}

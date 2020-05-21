# TouhouRPC
TouhouRPC is a program that creates a Discord Rich Presence status for Touhou games. 

## DLL files
Make sure the *discord_game_sdk.dll* file is in the same folder as the *TouhouRPC.exe* executable before executing it. If it doesn't, an error message will show up and the program will close.  
- You can use the .dll file available in the *ExternalLibs* folder of this repo. Use the x86 or x64 version depending on which platform you used when compiling the program.  
- You can also use the .dll file available in the Discord Game SDK zip file provided by Discord. You will find the dlls under the *lib* folder. ([Link to their documentation here.](https://discordapp.com/developers/docs/game-sdk/sdk-starter-guide))


## Supported games
| Game											| Basic (Menu + Stage and Bosses) | Extended (Practice, Replays, Resources) | Spell Card Practice | Music Room |
|--|--|--|--|--|
| Touhou 06 - Embodiment of Scarlet Devil		| ✅ | ❌ | n/a | ❌ |
| Touhou 07 - Perfect Cherry Blossom			| ✅ | ✅ | n/a | ✅ |
| Touhou 08 - Imperishable Night				| ✅ | ❌ | ✅ | ❌ |
| Touhou 09 - Phantasmagoria of Flower View		| ❌ | ❌ | n/a | ❌ |
| Touhou 10 - Mountain of Faith					| ❌ | ❌ | n/a | ❌ |
| Touhou 11 - Subterranean Animism				| ✅ | ❌ | n/a | ❌ |
| Touhou 12 - Undefined Fantastic Object		| ❌ | ❌ | n/a | ❌ |
| Touhou 12.8 - Great Fairy Wars				| ❌ | ❌ | n/a | ❌ |
| Touhou 13 - Ten Desires						| ✅ | ✅ | ✅ | ✅ |
| Touhou 14 - Double Dealing Character			| ✅ | ✅ | ✅ | ✅ |
| Touhou 15 - Legacy of Lunatic Kingdom			| ✅ | ✅ | n/a | ✅ |
| Touhou 16 - Hidden Stars in Four Seasons		| ✅ | ✅ | ✅ | ✅ |
| Touhou 17 - Wily Beast and Weakest Creature	| ✅ | ✅ | ✅ | ✅ |

Once the above are all complete, support may be added for the other danmaku side games (9.5/12.5/14.3/16.5). If you want any game not already supported, contributions are more than welcome!

## Display examples
![Th17 Example - Fighting](https://relick.me/touhourpc/1-wbawc-playing.png)
  
![Th07 Example - Menu](https://relick.me/touhourpc/2-pcb-menu.png)
  
![Th13 Example - Spell Card Practice](https://relick.me/touhourpc/3-td-spellcard.png)
  
![Th14 Example - Music Room](https://relick.me/touhourpc/4-ddc-musicroom.png)
  
![Th17 Example - Stage Practice](https://relick.me/touhourpc/5-wbawc-practicing.png)
 

## Additional notes
This program supports games patched using the [Touhou Community Reliant Automatic Patcher](https://github.com/thpatch/thcrap). It also supports games using the vpatch.

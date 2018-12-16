# Bloki
Bloki is a small project I started back in 2011 I think, as an exercise for writing a complete game. The challenge was creating a clone of Tetris with all the features a formal game could have like main menu, instructions, music, highscores and everything. Though is still a work in progress as it is missing still music and instructions (among other things).

It was written completely in C++ using de Windows' GDI+ library. All the art is original, including the main mascot Baro the Blokus.

## Building instructions
You shouldn't need to download additional dependencies as only GDI+ is needed and is included since Windows XP.
However, in case of problem, the runtime redistributable can be found [here](https://www.microsoft.com/en-us/download/details.aspx?id=18909).

Building was tested using Visual Studio 2017 Community Edition.

## About the game

The game starts with a Main Menu where you can select to start a new game, look at highscores or exit.

![Main menu screenshot][main_menu]

When starting a new game you control the bloki (plural for blokus) using the directional keys:
- Left or right: Moves the blokus horizontally
- Up: Rotates the blokus clockwise
- Down: Accelerates downward motion

![Gameplay][game_1]

While playing Baro will "interact" with the player depending on the status. If the player scores a point he will smile and wave his arms, if the player puts a block too high he will get nervous and if you lose he will get sad.

![Baro nervous][game_2]

![Game over][game_3]

It's a really simple game but I had a great time working on it. I learned that making a game from scratch is harder than I originally thought, but is a very rewarding experience. 

[main_menu]: screenshots/main_menu.png "Main menu"
[game_1]: screenshots/game_1.png "Normal game"
[game_2]: screenshots/game_2.png "Risky game"
[game_3]: screenshots/game_3.png "Game over"
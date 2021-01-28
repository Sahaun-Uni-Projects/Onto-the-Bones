# About

<b>Onto the Bones</b> is a turn-based tactical game where you have to guide yourself, the pug, to the bones.

It is not as easy as it sounds... you have to deal with bats, hounds, and the deadliest of them all, SNAILS, along the way.
<br/><br/>
<p align="center">
  <img src="https://user-images.githubusercontent.com/27750907/106031206-36f3c280-60f9-11eb-8020-4372d578592f.png">
</p>
<br/><br/>

# Characters

| Character        | HP            | Moves per Turn | Representation in Level Layout |
| :--------------: |:-------------:| :-------------:| :----------------------------: |
| Pug              | 1             | 2              | P                              |
| Bat              | 1             | 3              | a                              |
| Hound            | 2             | 2              | b                              |
| Snail            | 3             | 1              | c                              |

Additionally, the bone is represented with `G`, ground with `1` and empty space with `0`.
<br/><br/>

# Gameplay
The player and the enemies move in turns. You can skip the moves of the active turn using `Q`.

Stepping on a cell with the enemy/player damages them. (reduces heath points by 1)

Player and enemies can attack each other only if they are on adjacent cells and the attacker has moves remaining on this turn.

Step on the bone and press `E` to proceed to the next level!

If you are stuck, press `R` to restart the level.

Highscore is sorted by the greater number of kills and lesser number of tries.
<br/><br/>

# Inspiration
The game is inspired by <a href="https://store.steampowered.com/app/590380/Into_the_Breach">Into the Breach</a>.

Codewise, there are obvious indications of GameMaker Language (GML), the proprietary programming language of <a href="https://www.yoyogames.com/">GameMaker</a>.

# Contributors
1. Ahnaf Abid Khan
2. Shahbaj Tamjid
3. Shaharaj Uddin
4. Sohom Sahaun

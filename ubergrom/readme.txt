The black label and color label are just for you to see - Sticker Giant is printing them and will deliver to your address.

There are three files to burn:

For the AVR:
- burn SSA_AVR_FLASH to the flash section - it is 128K.
- burn SSA_AVR_EEPROM to the EEPROM section - without this the high score saving won't work
- Also look at SSA_AVR_ConfigTab - it's a screenshot of the XGPro software showing the switches that must be set (same as all previous UberGROMS)

For the Flash ROM:
- burn ssa_uber_rom512_8.bin to the 512k flash chip.

To test:

You should be able to start up the cart normally. It will add two options:

SUPER SPACE ACER '24 - this is the new game
LIBRARY - this has some old options to fill up the GROM:

1) Play 1992 version - this loads and plays the original release, same as is in Classic99
2) Read 1992 Story Text - this loads a text viewer to read the story that was in the original manual
3) See 1995 intro - In '95 I started a sequel - all I did was the title sequence. This is that.
4) Read 1995 Story Text - text viewer again.

QUIT should exit all these bonus apps.

For the new game, the main thing to check is whether the EEPROM was detected. When you get to the animated title page, just press FIRE on the title page and C to enter the config screen. Then try to clear the high scores - it will tell you if it can't find the EEPROM. If it can't, you might need to reprogram the EEPROM part.

-----
SOUND
-----

I wasn't able to test on hardware the extra sound options.

To test SID Blaster:
- this is the default mode, you can just play. If you hear sound effects as well as music, then it's working.

To test FORTI:
- on the ship select, press "C for CONFIG and MUSIC TEST"
- press 'B' to select MUSIC+SFX FORTI
- When you play, you should hear both music and sound effects
- Note if you select this without a FORTI, the music will be interrupted in odd ways by sound effects
- this setting is automatically saved when selected

To use the sound test:
- on the config and music test screen, you can press the keys 1-9 to play the various tunes.
- if you hold left (must be on a joystick) and press the keys 0-9, it will play sound effects on the currently selected device.
- again, the FORTI mode won't work if you don't have a FORTI (you probably won't get any sound effects at all in test mode).

-------
PLAYING
-------

To Play:
- it's a standard space shooter - shoot the enemies and don't get hit.
- some enemies will drop a powerup: 'S' gives you shields, 'P' powers up the pulse weapon, and 'W' powers up the wide shot. P and W will switch weapons if you are currently on the other one. It remembers the powerup level of the previous weapon if you switch back to it.
- each ship operates differently:
    - The cruiser has a permanent shield that can withstand 3 hits. The Shield powerup will restore one level of shield. The cruiser has only one life, made up for by the shields.
    - The snowball has a timed shield that is impervious to hits. The Shield powerup grants more time. Snowball has three lives.
    - The ladybird has a timed shield but it is offensive and gains power by ramming opponents. Ladybird has three lives.
- there are 5 stages, each ending with a more difficult boss. Shooting the boss will damage the body, granting access to the engines at the rear. Destroy all engines to defeat the boss.

Difficulty:
- In easy mode, there are only two enemies on screen. They can only aim in 8 directions when they fire at you.
- Medium mode increases the enemy count, and allows enemies to aim more precisely.
- Hard mode increases the enemy count again, allows accurate aim, and increases the aggression of the jet enemy.

All ships have a custom ending animation, but this is available ONLY in hard mode. In medium mode you only get the end credits, and in easy mode you only get a game over screen.

-------------------------------------
All secrets only work with a joystick
-------------------------------------

SECRET MODES:
- There are two secret ships and a third secret mode. To access them, hold fire while moving left or right on the ship select screen.
    - Gnat - this tiny ship has a minscule hit box. But it also has the weakest weapon in the game and no ability to power up. Gnat has three lives.
    - Selena - unicorn princess with powerful homing weapons, but only one life and no powerups.
    - Snowball Stealth mode - hold DOWN while starting a game with the snowball. All enemies will be black (on the black background of space).

---------
Classic99
---------

I don't intend to release this version for Classic99. In a few months we'll release the ROM version, which works without the UberGROM and loses the library and and high scores. But if you need to test, this is what goes into Classic99.ini:

[UserCart13]
; Ubergrom is used to boot and save high scores
name=Super Space Acer UberGROM
rom0=*|0|0|D:\work\TI\superspaceacer\ubergrom\ssa_uber_rom512_8.bin
rom1=U|0000|20000|D:\work\TI\superspaceacer\ubergrom\SSA_AVR_FLASH.bin
rom2=T|0000|1000|D:\work\TI\superspaceacer\ubergrom\ssa_eeprom_my_local.bin

You need to make a copy of SSA_AVR_EEPROM.BIN and rename it to ssa_eeprom_my_local.bin as above. (Or whatever filenames you like). This is because Classic99 will modify the file, and we should really just ship the original file.

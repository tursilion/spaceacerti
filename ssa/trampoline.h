#ifndef TRAMPOLINE_H
#define TRAMPOLINE_H

#include "game.h"
#include "highscores.h"

// trampoline functions
void wrapenemy();
void wrapnoen(int x);
void wrapplayer();
void wrapcheat();
void wrapcolchk(int x);
void wrapplycol();
void wrapplayerstraight();
void wrapplayerleft();
void wrapplayerright();
void wrapunpackboss(unsigned int level);
void winwrapgetscroll(unsigned char *dst, const char *src, unsigned int cnt);
char winwrapgetbyte(const char *adr);
void wrapstars();
void wrapinitstars();
void wrapispace();
void wrapbackground();
void wrapcheckdamage(int sr, int sc, int pwr);
void wrapLoadSelenaPic();
void wrapgetfontbytes(unsigned char *dest, const unsigned char *src, unsigned int cnt);
void wrapgetfontonebyte(unsigned char *dest, const unsigned char *src);
void wrapspritescore(unsigned int sprpat, unsigned int sprtab, unsigned int row, unsigned int col, unsigned int ch);
void wrapLoadEngineSprites();
void wrapPlayerFlameBig();
void wrapPlayerFlameSmall();
void wrapCopyShip(const unsigned char *p, const unsigned char *c, int idx);
void wrapGamWin();
void wrapLoadStoryFont();
void wrapLoadLadyScreen();
void wrapLadyBugByte(int off);
void wrapDrawLastRowText(int txt, unsigned int off);
void wraploadgnat1();
void wraploadgnat2();
void wraploadgnat3();
void wraploadgnat4();
void wraploadgnat5();
void wrapwarpout();
void wrapLoadFinalSnowball();
void wrapFinalSnowballBig();
void wrapFinalSnowballSmall();
void wrapldpic();
void wrapldcruiserend();
void wrapLoadSnowballBase();
unsigned int wrapLoadBossF18A(unsigned int n, unsigned int scanline);
void wrapLoadF18MainPalette();
void wrapInitCruiser();
void wrapInitSnowball();
void wrapInitLadybug();
void wrapInitGnat();
void wrapInitSelena();
void wrapLoadBossGfx();
void wrapRestoreBossGfx();
void wrapAddDestroyed(unsigned int ptr);
void wrapladybugwin();
void wrapbossdraw();
void wrapGameWinHard();
void wrapsoundtest();
void wrapRegisterHiScore();
int wrapCheckHighScores();
void wrapClearHighScores();
void wrapSaveScores(struct _scores *);

#endif

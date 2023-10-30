#ifndef HUMAN_H
#define HUMAN_H

void player();
void shoot();
void mvshot();
void homingshot();
void cheat();
void colchk(int half);
void plycol();
void pdie();
void pboom();
void playerstraight();
void playerleft();
void playerright();
void playerinit();
void dyen(unsigned int x);

void initCruiser();
void initSnowball();
void initLadybug();
void initGnat();
void initSelena();

// these are now just flags, though FLAME_BIG is used to init the sprite
// pattern 104 is no longer related though
#define FLAME_BIG 100
#define FLAME_SMALL 104

extern int pwrlvl;
extern int oldpwrlvl;
extern int flst;
#endif

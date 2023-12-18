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

void initCruiserBase();
void initSnowballBase();
void initLadybugBase();
void initGnatBase();
void initSelenaBase();

// these are now just flags, though FLAME_BIG is used to init the sprite
#define FLAME_BIG 2
#define FLAME_SMALL 0
#define FLAME_CHAR 100

extern int pwrlvl;
extern int oldpwrlvl;
extern int flst;
#endif

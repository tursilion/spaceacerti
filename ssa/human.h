#ifndef HUMAN_H
#define HUMAN_H

void player();
void shoot();
void mvshot();
void homingshot();
void cheat();
void colchk(uint8 half);
void plycol();
void pdie();
void pboom();
void playerstraight();
void playerleft();
void playerright();
void playerinit();
void dyen(unsigned char x);

// these are now just flags, though FLAME_BIG is used to init the sprite
// pattern 104 is no longer related though
#define FLAME_BIG 100
#define FLAME_SMALL 104

extern uint8 pwrlvl;
extern uint8 oldpwrlvl;
extern uint8 flst;
#endif

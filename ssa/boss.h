#ifndef BOSS_H
#define BOSS_H


// boss smooth scroll pattern table offset
#define SCROLL_OFFSET 0x0800

// boss data (BOSS start for ships, POD start for level 2 pods)
#define BOSS_START 121
extern const unsigned char BOSS1[],BOSS2[],BOSS3[],BOSS4[],BOSS5[];

void boss();
void drboss();
void erboss();
void mboss();
uint8 checkdamage(uint8 sr, uint8 sc, uint8 pwr);
void warpout();
void whoded();
void byboss();
void AddDamage(unsigned int vptr);
void AddDamageF18a(unsigned int vptr);
void AddDestroyed(unsigned int vptr);
void AddDestroyedF18a(unsigned int vptr);
void PrepareBoss(unsigned char idx, unsigned char r);

#endif
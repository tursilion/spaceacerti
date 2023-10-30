// enable this for a test mode - ships will fire constantly
// collisions are checked, but player never dies. You can
// start a game at high speed with the debugger open and
// just watch for VDP overruns
//#define TEST_MODE

// libti99
#include <vdp.h>
#include <sound.h>
#include <kscan.h>

// game
#include "game.h"
#include "trampoline.h"
#include "enemy.h"
#include "human.h"
#include "music.h"
#include "f18load.h"

// bg.c
extern unsigned char screenFlashCnt;

// color of player's ship (was white)
unsigned int playerColor;
int shotOffset;

unsigned int joynum;
int killedby,flst;		// enemy who hit us, flame status
int pwrlvl;				// pwrlvl: 0-2 = pulse wave level 1,2,3, 4-6=3-way level 1,2,3  (3 is gnat)
int oldpwrlvl;
int lives;
unsigned int score, oldscore;	// oldscore is used as a temporary during demo play
unsigned int scoremode;		// indicates bonus modes played via score's last digit. 0=normal, 1=gnat, 2=Selena, 3=invisible enemies
int shr[NUM_SHOTS+1], shc[NUM_SHOTS];	// player shots row and col - shr is plus 1 so the last index can always be 0, faster searches
int shd[NUM_SHOTS];					// player shot x direction (y is constant)
int shrd[NUM_SHOTS];					// player shot y direction (only for selena)
unsigned int shield;
unsigned int hittime;				// mostly for cruiser - shakes the ship when hit
int pcr4,ptp4,pr4,pc4,p4Time;		// powerup settings
unsigned int CanNotShoot;
unsigned int playerXspeed, playerYspeed;

// superspaceacer.c
extern void (*shieldsOn)();
extern void (*shieldsOff)();

void mvshot()
{ /* move the player's shots */

#if 1
	// WARNING: assumes NUM_SHOTS to be 9
	// constant offsets are more efficient than array lookups by a lot in SDCC
#define MOVE_ONE_SHOT(a)								\
	if (shr[a]) {										\
		shr[a]-=8;										\
		shc[a]+=shd[a];									\
		if ((shr[a]>191)||(shc[a]>250)||(shc[a]<5)) {	\
            if (force == 2) force = 0;                  \
			spdel(PLAYER_SHOT+a);						\
			shr[a]=0;									\
		} else {										\
			sploct(PLAYER_SHOT+a,shr[a],shc[a]);		\
		}												\
	}
	MOVE_ONE_SHOT(0);
	MOVE_ONE_SHOT(1);
	MOVE_ONE_SHOT(2);
	MOVE_ONE_SHOT(3);
	MOVE_ONE_SHOT(4);
	MOVE_ONE_SHOT(5);
	MOVE_ONE_SHOT(6);
	MOVE_ONE_SHOT(7);
	MOVE_ONE_SHOT(8);

#undef MOVE_ONE_SHOT

#else
	uint8 a;
	for (a=0; a<NUM_SHOTS; a++) {
		if (shr[a]) { 
			shr[a]-=8;
			shc[a]+=shd[a];
			if ((shr[a]>191)||(shc[a]>250)||(shc[a]<5)) { 
				spdel(PLAYER_SHOT+a);  
				shr[a]=0; 
			} else {
				sploct(PLAYER_SHOT+a,shr[a],shc[a]);
			}
		}
	}
#endif
}

// a version of target (see superspaceacer.c) that works with the negative row of boss engines,
// for Selena's homing shots
char rowtarget(unsigned char dest, unsigned char src)
{ 
	// similar to sgn(), but designed to work with unsigned values
	// without wraparound
    if (dest > 225) dest=0;
	if (dest > src) return 1;
	if (dest < src) return -1;
	return 0;
}
 
void homingshot() {
    /* move the princess shots */

	// WARNING: assumes NUM_SHOTS to be 9
	// constant offsets are more efficient than array lookups by a lot in SDCC
#define MOVE_JUST_UP(a)                                 \
	if (shr[a]) {										\
        if (shrd[a]>-8) shrd[a]--;                      \
        if (shd[a]>0) shd[a]--;                         \
        if (shd[a]<0) shd[a]++;                         \
		shr[a]+=shrd[a];								\
        shc[a]+=shd[a];                                 \
		if ((shr[a]==0)||(shr[a]>191)||(shc[a]>250)||(shc[a]<5)) {	\
            if (force == 2) force = 0;                  \
			spdel(PLAYER_SHOT+a);						\
			shr[a]=0;									\
		} else {										\
			sploct(PLAYER_SHOT+a,shr[a],shc[a]);		\
		}												\
	}

#define MOVE_ONE_SHOT(a)								\
	if (shr[a]) {										\
        shrd[a]+=rowtarget(enr[en],shr[a]);             \
        shd[a]+=target(enc[en],shc[a]);                 \
		shr[a]+=shrd[a];								\
		shc[a]+=shd[a];									\
		if ((shr[a]==0)||(shr[a]>191)||(shc[a]>250)||(shc[a]<5)) {	\
            if (force == 2) force = 0;                  \
			spdel(PLAYER_SHOT+a);						\
			shr[a]=0;									\
		} else {										\
			sploct(PLAYER_SHOT+a,shr[a],shc[a]);		\
		}												\
	}

    // find the first available enemy
    unsigned char en = 0;
    while ((en<6)&&(ent[en]<ENEMY_ENGINE)) ++en;
    if (en >= 6) {
	    MOVE_JUST_UP(0);
	    MOVE_JUST_UP(1);
	    MOVE_JUST_UP(2);
	    MOVE_JUST_UP(3);
	    MOVE_JUST_UP(4);
	    MOVE_JUST_UP(5);
	    MOVE_JUST_UP(6);
	    MOVE_JUST_UP(7);
	    MOVE_JUST_UP(8);
    } else {
	    MOVE_ONE_SHOT(0);
	    MOVE_ONE_SHOT(1);
	    MOVE_ONE_SHOT(2);
	    MOVE_ONE_SHOT(3);
	    MOVE_ONE_SHOT(4);
	    MOVE_ONE_SHOT(5);
	    MOVE_ONE_SHOT(6);
	    MOVE_ONE_SHOT(7);
	    MOVE_ONE_SHOT(8);
    }

#undef MOVE_ONE_SHOT
#undef MOVE_JUST_UP

}

void colchk(int half) { 
	/* check sprite collisions against player shots */
	int a,b,k,s;
	int k1,k2;

	// lopsided halves :)
	if (half) {
		k1=0; k2=4;
	} else {
		k1=4; k2=6;
	}

	for (k=k1; k<k2; k++) {
		if ((ent[k] >= ENEMY_SAUCER) && (ent[k] < ENEMY_MAX)) {
			for (s=0; s<NUM_SHOTS; s++) {
				if (shr[s]) {
					a=abs(enr[k]-shr[s]); 
					b=abs(enc[k]-shc[s]);
					if ((a<=15)&&(b<=15)) { 
						dyen(k);
						spdel(s+PLAYER_SHOT);
						shr[s]=0;
					}
				}
			}
		}
	}
}

void plycol() {
	unsigned int x,y;
	unsigned int r,c;
	int a;

	/*check player collisions */
	spposn(PLAYER_SPRITE,r,c);
	r+=playerOffset;
	c+=8;
	
	/*pwrup?*/
	if (ptp4 != POWERUP_NONE) { 
		y=abs(pr4-r); x=abs(pc4-c);
		if ((x<=16)&&(y<=16)) {
			// Player collides with powerup
			if (ptp4==POWERUP_SHIELD) { 
				// Shield power increase
                playsfx_shieldup();
				if (playership == SHIP_CRUISER) {
					if (shield < 75) shield += 25;	// maximum shields set on Cruiser
				} else {
					if (nDifficulty == DIFFICULTY_EASY) shield+=50;
					if (nDifficulty <= DIFFICULTY_MEDIUM) shield+=50;
					shield+=100;
				}
			} else if (ptp4 == POWERUP_WAVE) {
                playsfx_pwrpulse();
				if ((pwrlvl&0x0f) < PWR3WAY) {
					if ((pwrlvl&0x03) < 2) {
						pwrlvl++;
					}
				} else {
                    // swapping
                    uint8 tmp = pwrlvl;
					pwrlvl=oldpwrlvl;
                    oldpwrlvl = tmp;
				}
			} else if (ptp4 == POWERUP_3WAY) {
                playsfx_pwrwide();
				if ((pwrlvl&0x0f) >= PWR3WAY) {
					if ((pwrlvl&0x03) < 2) {
						pwrlvl++;
					}
				} else {
                    // swapping
                    uint8 tmp = pwrlvl;
					pwrlvl=oldpwrlvl;
                    oldpwrlvl = tmp;
				}
			}
			// remove powerup
			ptp4=POWERUP_NONE;
			spdel(POWERUP_SPRITE);
			addscore(2);
		}
	}

	/*enemies?*/
	if (playership != SHIP_GNAT) {
		for (a=0; a<12; a++) {
			if ((ent[a]>ENEMY_EXPLOSION)&&(ent[a]<ENEMY_MAX)&&(ent[a]!=ENEMY_ENGINE)) { 
				y=abs(enr[a]-r); x=abs(enc[a]-c);
				if ((x<=8)&&(y<=8)&&(hittime==0)) {
					if (shield < 1) {
						killedby=a;
#ifndef TEST_MODE
						a=99;
#endif
					} else if (playership == SHIP_CRUISER) {
                        playsfx_shielddown();
						shield -= 25;
						if (shield > 75) shield = 0;
                        if (shield == 0) playsfx_shieldwarn();
						hittime = 10;
						// kill off the enemy
						if (ent[a] < ENEMY_SAUCER) {
							wrapnoen(a);
                            if (en_func[a]==enemyhominglaser) {
                                // special case, we need to leave it active so the trail fades out
                                // importantly, noen does not delete the function pointer
                                // change type so we keep being called
			                    ent[a]=ENEMY_EXPLOSION;
                            }
						} else {
							ep[a]=0;
							dyen(a);
						}
						playsfx_shipdead();
					} else if (playership == SHIP_LADYBUG) {
						if (ent[a] >= ENEMY_SAUCER) {
                            playsfx_shieldup();
							ep[a]=0;
							dyen(a);
							shield+=100;	// ladybug recharges shield by hitting enemies
						} else {
							wrapnoen(a);
                            if (en_func[a]==enemyhominglaser) {
                                // special case, we need to leave it active so the trail fades out
                                // importantly, noen does not delete the function pointer
                                // change type so we keep being called
			                    ent[a]=ENEMY_EXPLOSION;
                            }
						}
					} else {
						// still kill off whatever we ran into
						if (ent[a] >= ENEMY_SAUCER) {
							ep[a]=0;
							dyen(a);
						} else {
							wrapnoen(a);
                            if (en_func[a]==enemyhominglaser) {
                                // special case, we need to leave it active so the trail fades out
                                // importantly, noen does not delete the function pointer
                                // change type so we keep being called
			                    ent[a]=ENEMY_EXPLOSION;
                            }
						}
					}
				}
			}
		}
	} else {
		for (a=0; a<12; a++) {
			if ((ent[a]>ENEMY_EXPLOSION)&&(ent[a]<ENEMY_MAX)) { 
				y=abs(enr[a]-r); x=abs(enc[a]-c);
				if ((x<=3)&&(y<=2)) {
					// this is the gnat, so there's no point checking the cruiser shields ;)
					if (shield < 1) {
						killedby=a;
#ifndef TEST_MODE
						a=99;
#endif
					} else {
						if (ent[a] >= ENEMY_SAUCER) {
							ep[a]=0;
							dyen(a);
						} else {
							wrapnoen(a);
                            if (en_func[a]==enemyhominglaser) {
                                // special case, we need to leave it active so the trail fades out
                                // importantly, noen does not delete the function pointer
                                // change type so we keep being called
			                    ent[a]=ENEMY_EXPLOSION;
                            }
						}
					}
				}
			}
		}
	}
	if (a>=99) pdie();
}
 
void pdie()
{
	/* player dies */
	unsigned char oldflag=flag;

	shutup();
	pboom();
	spdall();
	delaystars(40);		// really a wrap function
	lives--;
	if (playership != SHIP_GNAT) {
		pwrlvl=PWRPULSE;
        oldpwrlvl=PWR3WAY;
		if (playership == SHIP_SELENA) pwrlvl+=2;
	} else {
		pwrlvl=PWRGNAT;
        oldpwrlvl=PWRGNAT;
	}
	if (lives<0) {
		flag=PLAYER_DIED;
	} else {
		flag=MAIN_LOOP_ACTIVE;
		if ((oldflag==MAIN_LOOP_ACTIVE) || (oldflag == MAIN_LOOP_DONE)) {
			// ispace is in the fixed bank, but it changes banks many times
			wrapispace();
		}
	}
}
 
void pboom()
{
	/* do SSA ship explode routine */
	unsigned char qw;
	int lp1, lp2;		// used to move instead of automotion

	// delete 'boss approaching' if it's up
	if (flag != BOSS_LOOP_ACTIVE) {
		// the blimp gets corrupted by this - any other time worst case is we lose a star, I don't care about that :)
		hchar(11, 0, 32, 32);
	}
	DelSprButPlayer(killedby+ENEMY_SPRITE);

	// stop audio
	shutup();

	for (qw=1; qw<4; qw++) { 
		wrapstars();							// move stars
		spcolr(PLAYER_SPRITE,playerColor); 
		spcolr(PLAYER_SPRITE+1,playerColor);	// set ship normal
		spcolr(PLAYER_SPRITE+2,playerColor); 
		spcolr(PLAYER_SPRITE+3,playerColor);
		wrapplayerstraight(); delaystars(4);		// straight
		wrapplayerleft(); delaystars(4);			// tilt left
		spcolr(PLAYER_SPRITE,9); 
		spcolr(PLAYER_SPRITE+1,9);				// set ship lt red
		spcolr(PLAYER_SPRITE+2,9); 
		spcolr(PLAYER_SPRITE+3,9);
		wrapplayerstraight(); delaystars(4);		// straight
		wrapplayerright(); delaystars(4);			// tilt right
	}

	SOUND=0xe6;									// force a noise sound for explosion
	SOUND=0xf0;

	if (playership != SHIP_GNAT) {
		patsprcpy(EXPLOSION_COPY,248);					// copy the boss explosion character in to sprite space
	} else {
		// gnat gets tiny explosion sprites too
		vdpmemset(248*8+0x0800, 0, 32);					// zero the pattern
		vdpchar(248*8+0x0800+8, 0x01);					// one pixel near the center
	}
	sppat(PLAYER_SPRITE,248); sppat(PLAYER_SPRITE+1,248);			// overwrite all four player sprites
	sppat(PLAYER_SPRITE+2,248); sppat(PLAYER_SPRITE+3,248);			// shield is off, so we ignore it

	// fake the automotion for a few frames
	for (lp1=1; lp1<16; lp1++)
	{ 
		unsigned char r,c;
		for (lp2=PLAYER_SPRITE; lp2<PLAYER_SPRITE+4; lp2++) {
			SOUND=0xf0+lp1;
			spposn(lp2, r, c);
			if (lp2&1) {
				c+=4;
			} else {
				c-=4;
			}
			if (lp2&2) {
				r+=4;
			} else {
				r-=4;
			}
			sploct(lp2, r, c);
		}
		delaystars(2);
	}
}
 
void dyen(unsigned int x) { 
	/* enemy has been shot */
	uint8 r,c,k;

	if (ep[x] > 0) {
		// count down armor
		ep[x] -= damage[pwrlvl&0x07];
	}
	if (ep[x] <= 0) {
		// no armor left
		addscore(ent[x]);
		playsfx_shipdead();
		if (ent[x] == ENEMY_BEAMGEN) {
			// figure out which one we destroyed
			// based on which character it is, we know if it's left or right

			// make bullet into explosion
			// Possible bug: we assume we own the bullet by now. Only the helicopter
			// can steal our bullet, and it shouldn't last that long...
			ent[x+6] = ENEMY_EXPLOSION;
			enr[x+6]=enr[x];
			enc[x+6]=enc[x];
			ech[x+6]=52;
			eec[x+6]=72;
			esc[x+6]=52;
			en_func[x+6] = enemyexplosion;
			sprite(x+6+ENEMY_SPRITE,ech[x+6], f18a?PAL_EXPLODE:COLOR_LTRED, enr[x+6], enc[x+6]);

			if (ech[x] == 76) {
				// left destroyed
				// make active on right
				enc[x]+=96;
				ech[x]=eec[x];
			} else {
				// right destroyed
				enc[x]-=96;
				ech[x]=esc[x];
			}
			ent[x]=ENEMY_DEADBEAM;
			en_func[x]=enemydeadbeam;
			sppat(x+ENEMY_SPRITE, ech[x]);
			sploct(x+ENEMY_SPRITE, enr[x], enc[x]);
		} else if (ent[x] != ENEMY_BOMB) {
			ent[x]=ENEMY_EXPLOSION;
			ech[x]=52;
			eec[x]=72;
			esc[x]=52;
			en_func[x] = enemyexplosion;
			spcolr(x+ENEMY_SPRITE,f18a?8:COLOR_LTRED);
		} else {
			playsfx_nukebomb(); // overrides previous
			spposn(x+ENEMY_SPRITE,r,c);
			screen(COLOR_WHITE);
			screenFlashCnt = 4;
			for (k=0; k<12; k++) {
				wrapnoen(k);
			}
			for (k=0; k<9; k++)	{ 
				ent[k]=ENEMY_SHOT;
				en_func[k]=enemyshot;
				enr[k]=r; enc[k]=c;
				ech[k]=84;
				eec[k]=84;
				esc[k]=84;
				sprite(k+ENEMY_SPRITE,84,f18a?PAL_MINETIP:COLOR_LTYELLOW,r,c);  // this is the explosion particle - a shot with a recycled palette
				if (k<3) {
					ers[k]=-9;
				}
				if ((k>2)&&(k<6)) {
					ers[k]=0;
				}
				if (k>5) {
					ers[k]=9;
				}
				if (k%3==0) {
					//ecs[k]=-9;        // TODO: COMPILER CRASHES ON THIS LINE - invalid expression as operand
				}
				if ((k-1)%3==0) {
					ecs[k]=0;
				}
				if ((k+1)%3==0) {
					ecs[k]=9;
				}
			}
			ent[4]=ENEMY_EXPLOSION;
			en_func[4] = enemyexplosion;
			ech[4]=52;
			eec[4]=72;
			esc[4]=52;
			sprite(ENEMY_SPRITE+4,52,f18a?PAL_EXPLODE:COLOR_LTRED,r,c);
			SOUND=0xff;
		}
	} else {
        playsfx_armor();
    }

}

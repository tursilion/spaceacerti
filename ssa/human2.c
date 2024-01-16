// libti99
#include <vdp.h>
#include <sound.h>
#include <kscan.h>
#include <f18a.h>

// game
#include "game.h"
#include "trampoline.h"
#include "enemy.h"
#include "human.h"
#include "music.h"
#include "f18load.h"

extern unsigned int CanNotShoot;
extern unsigned int hittime;				// mostly for cruiser - shakes the ship when hit
extern void (*shieldsOn)();
extern void (*shieldsOff)();
extern int shd[NUM_SHOTS];					// player shot x direction (y is constant)
extern int shrd[NUM_SHOTS];					// player shot y direction (only for selena)


// play shot types
#define PLAYER_SHOT_PULSE_BASE 88
#define PLAYER_SHOT_3WAY_STRAIGHT	208
#define PLAYER_SHOT_3WAY_LEFT1		212
#define PLAYER_SHOT_3WAY_LEFT2		216
#define PLAYER_SHOT_3WAY_RIGHT1		220
#define PLAYER_SHOT_3WAY_RIGHT2		224

// keypad keys for various cheats
#define CHEAT_NOSHIELD	'0'
#define CHEAT_SHIELDS	'2'
#define CHEAT_LIVES		'3'
#define CHEAT_BOSSWARP	'5'
#define CHEAT_POWERUP	'6'
#define CHEAT_KILLBOSS	'7'
#define CHEAT_OFF		'8'

// kind of a sine table, generated with blassic to look like joystick outputs
const signed char SINEISH[128] = {
	0,0,0,0,4,4,4,4,4,0,0,0,0,4,4,4,4,0,0,0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,0,0,0,4,4,4,4,0,0,0,0,4,4,4,4,0,0,0,0,0,0,0,-4,-4,-4,-4,0,0,0,
	-4,-4,-4,-4,-4,-4,0,0,0,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,
	-4,-4,-4,-4,-4,-4,-4,-4,0,0,0,0,-4,-4,-4,-4,-4,-4,0,0,0,-4,-4,0,0,0
};

void player()
{ /* move the player based on joystick 'joynum' */
	// animate flame
	flst = (++flst)&0x03;
	if (playership == SHIP_SELENA) {
		// wing animation for Selena - we do the other ships below so we can use the Y axis to alter the flame
		switch (flst) {
		case 0:
		case 1:
			wrapplayerstraight();
			break;
		case 2:
			wrapplayerleft();
			break;
		case 3:
			wrapplayerright();
			break;
		}
	}

	// read controller
	if (joynum) {
        // human mode
		kscanfast(joynum);
		joystfast(joynum);

        // pause
        if (KSCAN_KEY == ',') {
			const unsigned char *oldLoop = pLoopMus;

			shutup();
			centr(11, "PAUSE");

			// wait for release
			do {
				kscanfast(joynum);
			} while (KSCAN_KEY == ',');

			// wait for new press
			do {
				kscanfast(joynum);
			} while (KSCAN_KEY != ',');

			centr(11, "     ");

			// wait for release
			do {
				kscanfast(joynum);
			} while (KSCAN_KEY == ',');

			pLoopMus = oldLoop;
		}
	} else {
        // attract mode

		static unsigned char xcnt=0, ycnt=0;
		kscanfast((xcnt&1)+1);
		if (KSCAN_KEY == JOY_FIRE) {
			score = oldscore;
			reboot();
		}

		KSCAN_KEY = JOY_FIRE;
		KSCAN_JOYX = SINEISH[xcnt>>1];
		KSCAN_JOYY = SINEISH[ycnt>>1];
		xcnt+=3;    // must be odd
		ycnt+=7;

        // if we are the ladybug and we have a shield, move towards enemies
        if ((playership == SHIP_LADYBUG) && (shield > 0)) {
            // find the first available enemy
            unsigned char en = 0;
            while ((en<6)&&(ent[en]<=ENEMY_ENGINE)) ++en;
            if (en < 6) {
			    if (enr[en] > SHIP_R+playerOffset+8) KSCAN_JOYY=-4;
			    if (enr[en] < SHIP_R+playerOffset-8) KSCAN_JOYY=4;
			    if (enc[en] > SHIP_C+8+8) KSCAN_JOYX=4;
			    if (enc[en] < SHIP_C-8+8) KSCAN_JOYX=-4;
            }
            if (flag!=BOSS_LOOP_ACTIVE) {
                // don't shoot unless we are at a boss
                KSCAN_KEY = 0xff;
            }
        }
        
        // but if there is a powerup, move towards that instead
		// we're on easy, so it might be safe ;)
		if (ptp4 != POWERUP_NONE) {
			if (pr4 > SHIP_R+playerOffset+8) KSCAN_JOYY=-4;
			if (pr4 < SHIP_R+playerOffset-8) KSCAN_JOYY=4;
			if (pc4 > SHIP_C+8+8) KSCAN_JOYX=4;
			if (pc4 < SHIP_C+8-8) KSCAN_JOYX=-4;
		}

        // don't move up when boss is approaching, only down
        if ((distns < 10)&&(flag!=BOSS_LOOP_ACTIVE)&&(KSCAN_JOYY==4)) KSCAN_JOYY=0;
	}
	if (KSCAN_JOYX == JOY_LEFT) {
		if (playership != SHIP_SELENA) {
			wrapplayerleft();
		}
		if (SHIP_C > playerXspeed) SHIP_C-=playerXspeed;
	} else if (KSCAN_JOYX == JOY_RIGHT) {
		if (playership != SHIP_SELENA) {
			wrapplayerright();
		}
		if (SHIP_C < 224-playerXspeed) SHIP_C+=playerXspeed;
	} else {
		if (playership != SHIP_SELENA) {
			wrapplayerstraight();
		}
	}

	if (hittime) {
		// impact rocking for cruiser
		--hittime;
		switch (hittime&0x03) {
		case 0:	wrapplayerleft(); break;
		case 1: wrapplayerstraight(); break;
		case 2: wrapplayerright(); break;
		case 3: wrapplayerstraight(); break;
		}
	}

	if (KSCAN_JOYY == JOY_UP) {
		if (SHIP_R > playerYspeed+18) {
			SHIP_R-=playerYspeed;
			flst=FLAME_BIG;
		}
	} else if (KSCAN_JOYY == JOY_DOWN) {
		if (SHIP_R < 151-playerYspeed) {
			SHIP_R+=playerYspeed;
			flst=FLAME_SMALL;
		}	
	}

	if ((shield) && (playership != SHIP_CRUISER)) {
        if (shield == 42) playsfx_shieldwarn();
		--shield;
		// ladybug counts down faster, but has an offensive shield that can ram enemies
		// can't ram the boss engines, but can ram the boss itself ;)
		if ((shield) && (playership == SHIP_LADYBUG)) {
            if (shield == 42) playsfx_shieldwarn();
			--shield;
			wrapcheckdamage(SHIP_R+playerOffset, SHIP_C+8, 0);	// add damage to boss body
		}
	}
    // these functions do nothing if player is Selena, so we don't need to check
	if (flst&FLAME_BIG) {
		wrapPlayerFlameBig();
	} else {
		wrapPlayerFlameSmall();
	}
	playmv();

	// helper for the shooting code
	if (CanNotShoot) {
		CanNotShoot--;
	}
	if (!CanNotShoot) {
#ifdef TEST_MODE
		shoot();
#endif
		
		if (KSCAN_KEY == JOY_FIRE) {
            if (force == 1) force = 0;
			shoot();
		}
	}
}

void shoot()
{ 
	uint8 a,a2,a3;
	uint8 truepwr = pwrlvl & 0x0f;
	int r,c;

	/* make player shoot if a shot available */

	// set up the ROF delay
	switch (pwrlvl & 0x03) {
		case 0x00:
			// slowest
			CanNotShoot=4;
			break;

		case 0x01:
			// medium
			CanNotShoot=3;
			break;

		case 0x02:
		case 0x03:	
			// fastest
			CanNotShoot=2;
			break;
	}

	/* take pwrlvl into account */
	if (truepwr >= PWR3WAY) { 
		// 3-way shot - we need two or three available, depending on frame and power
		a=0;
		while (shr[a]!=0) a++;

		if ((truepwr == PWR3WAY) || ((truepwr == PWR3WAY+2) && (0 == (pwrlvl&PWRFRAME)))) {
			// only 2 shots needed
			a2=a;
		} else {
			a2=a+1;
			while (shr[a2]!=0) a2++;
		}
		a3=a2+1;
		while (shr[a3]!=0) a3++;
		// only need to test the last one!
		if (a3 < NUM_SHOTS) {
			// update the frame bit (used to alternate 3way)
			pwrlvl ^= PWRFRAME;
			spposn(PLAYER_SPRITE,r,c);
			c+=8;	// recenter
			r+=shotOffset;
			if (a2 == a) {
				// only two slightly angled shots
				sprite(PLAYER_SHOT+a,PLAYER_SHOT_3WAY_LEFT1,f18a?PAL_SPREAD:COLOR_DKGREEN,r,c);
				sprite(PLAYER_SHOT+a3,PLAYER_SHOT_3WAY_RIGHT1,f18a?PAL_SPREAD:COLOR_DKGREEN,r,c);
				shd[a]=-2;
				shd[a3]=2;
				shr[a]=r;
				shr[a3]=r;
				shc[a]=c;
				shc[a3]=c;
			} else {
				// three wider spread shots
				shd[a]=-4;
				shd[a2]=0;
				shd[a3]=4;
				shr[a]=shr[a2]=shr[a3]=r;
				shc[a]=shc[a2]=shc[a3]=c;
				sprite(PLAYER_SHOT+a,PLAYER_SHOT_3WAY_LEFT2,f18a?PAL_SPREAD:COLOR_DKGREEN,r,c);
				sprite(PLAYER_SHOT+a2,PLAYER_SHOT_3WAY_STRAIGHT,f18a?PAL_SPREAD:COLOR_DKGREEN,r,c);
				sprite(PLAYER_SHOT+a3,PLAYER_SHOT_3WAY_RIGHT2,f18a?PAL_SPREAD:COLOR_DKGREEN,r,c);
			}
		}
	} else {
		// pulse cannon shot (also default for Selena)
		a=0;
		while (shr[a]!=0) a++;
		if (a!=NUM_SHOTS) { 
			spposn(PLAYER_SPRITE,r,c);
			r+=shotOffset;
			c+=8;
			shr[a]=r;
			shc[a]=c;
            shrd[a]=-4;
			shd[a]=0;
			sprite(PLAYER_SHOT+a,PLAYER_SHOT_PULSE_BASE+(truepwr<<2),f18a?PAL_PULSE:COLOR_MAGENTA,r,c);
		}
	}
}

void playerinit() {
	unsigned char a;

    // playerColor is already adjusted for the sake of F18A,
    // and the patterns are copied in as per normal, but the
    // presence of absence of the shield sprites differs
	sprite(PLAYER_SPRITE,108,playerColor,192,112);
	sprite(PLAYER_SPRITE+1,112,playerColor,192,128);
	sprite(PLAYER_SPRITE+2,116,playerColor,208,112);
	sprite(PLAYER_SPRITE+3,120,playerColor,208,128);
    if (f18a) {
        spdel(PLAYER_SHIELD);
        spdel(PLAYER_SHIELD+1);
        spdel(PLAYER_SHIELD+2);
        spdel(PLAYER_SHIELD+3);
    	sprite(PLAYER_FLAME,FLAME_CHAR,PAL_SHIPFLAME,224,120);
    } else {
	    sprite(PLAYER_SHIELD,124,COLOR_BLACK,192,112);
	    sprite(PLAYER_SHIELD+1,128,COLOR_BLACK,192,128);
	    sprite(PLAYER_SHIELD+2,132,COLOR_BLACK,208,112);
	    sprite(PLAYER_SHIELD+3,136,COLOR_BLACK,208,128);
    	sprite(PLAYER_FLAME,FLAME_CHAR,COLOR_LTYELLOW,224,120);
    }
	oldshield = 0;		// force an update for cruiser

	for (a=1; a<10; a++)	{ 
		SHIP_R-=6;
		playmv();
		wrapstars();
	}

	for (a=0; a<NUM_SHOTS; a++) {
		shr[a]=0;
	}

	flst=0;
	CanNotShoot=0;
	ptp4=POWERUP_NONE;
	hittime = 0;
	if (playership != SHIP_CRUISER) {
		// set initial shield
		shield=20;
	}
}

void initCruiserBase() {
    playerColor = COLOR_MEDRED;
	shieldsOn = shieldCruiser;
	shieldsOff = deShieldCruiser;

    wrapPlayerFlameSmall();
	
	playerOffset = 8;
	shotOffset = -7;
	playerXspeed = 10;
	playerYspeed = 6;

    if (f18a) {
        flameOffset = 30;
    }
}

void initSnowballBase() {
    shieldsOn = shieldSnowball;
	shieldsOff = deShieldSnowball;
    playerColor = COLOR_GRAY;

    wrapPlayerFlameSmall();

	playerOffset = 8;
	shotOffset = -8;
	playerXspeed = 12;
	playerYspeed = 8;
}

void initLadybugBase() {
    // f18 will override
    shieldsOn = shieldLadybug;
	shieldsOff = deShieldLadybug;
	playerColor = COLOR_MEDRED;

    wrapPlayerFlameSmall();

	playerOffset = 16;
	shotOffset = 16;

	playerXspeed = 12;
	playerYspeed = 8;
}

void initGnatBase() {
    // f18 will override
	shieldsOn = shieldGnat;
	shieldsOff = deShieldGnat;
	playerColor = COLOR_MEDGREEN;
    if (!f18a) {
        vdpmemset(100*8+0x0800, 0, 4*8);					// zero the flame sprites by default
    }

    playerOffset = 22;
	shotOffset = 23;

	playerXspeed = 12;
	playerYspeed = 8;
}

void initSelenaBase() {
    if (!f18a) {
        vdpmemset(100*8+0x0800, 0, 4*8);					// zero the flame sprites
    }
    // f18 will override
    shieldsOn = shieldSelena;
	shieldsOff = deShieldSelena;
	playerColor = COLOR_LTBLUE;

	playerOffset = 8;
	shotOffset = -8;

	playerXspeed = 12;
	playerYspeed = 8;
}

void cheat() { 
	/* process debugging keys */
	switch (KSCAN_KEY) {
		case CHEAT_SHIELDS:
            playsfx_shieldup();
			if (playership == SHIP_CRUISER) {
				shield+=25;
			} else {
				shield=shield+100;
			}
			break;

		case CHEAT_LIVES:
			lives++;
			break;

		case CHEAT_NOSHIELD:
			shield=0;
			break;

		case CHEAT_BOSSWARP:
			distns=1;
			break;

		case CHEAT_POWERUP:
			++pwrlvl; 
			if (pwrlvl>=7) {
				pwrlvl=0; 
			}
            if (pwrlvl >= PWR3WAY) {
                playsfx_pwrwide();
            } else {
                playsfx_pwrpulse();
            }
			break;

		case CHEAT_KILLBOSS:
			ep[0]=0; 
			ep[1]=0; 
			ep[2]=0;
			break;

		case CHEAT_OFF:
			ch=0;
			break;
	}
}
 
void playerstraight()
{	
	/*define ship shape 1 - straight */
	sppat(PLAYER_SPRITE,108); sppat(PLAYER_SPRITE+1,112);
	sppat(PLAYER_SPRITE+2,116); sppat(PLAYER_SPRITE+3,120);
	sppat(PLAYER_SHIELD,124); sppat(PLAYER_SHIELD+1,128);
	sppat(PLAYER_SHIELD+2,132); sppat(PLAYER_SHIELD+3,136);
}
 
void playerleft()
{ 
	/*shape 2 - left */
	sppat(PLAYER_SPRITE,140); sppat(PLAYER_SPRITE+1,144);
	sppat(PLAYER_SPRITE+2,148); sppat(PLAYER_SPRITE+3,152);
	sppat(PLAYER_SHIELD,156); sppat(PLAYER_SHIELD+1,160);
	sppat(PLAYER_SHIELD+2,164); sppat(PLAYER_SHIELD+3,168);
}
 
void playerright()
{	
	/*#3 - right */
	sppat(PLAYER_SPRITE,172); sppat(PLAYER_SPRITE+1,176);
	sppat(PLAYER_SPRITE+2,180); sppat(PLAYER_SPRITE+3,184);
	sppat(PLAYER_SHIELD,188); sppat(PLAYER_SHIELD+1,192);
	sppat(PLAYER_SHIELD+2,196); sppat(PLAYER_SHIELD+3,200);
}
 

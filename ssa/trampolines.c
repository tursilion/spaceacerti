// libti99
#include <vdp.h>
#include <f18a.h>
#include <sound.h>
#include <kscan.h>

// game
#include "game.h"
#include "trampoline.h"
#include "enemy.h"
#include "human.h"
#include "boss.h"
#include "f18load.h"
#include "highscores.h"

#define BIN2INC_HEADER_ONLY
#include "selena_end_c.c"
#include "selena_end_p.c"
#include "ladybugc.c"
#include "ladybugp.c"
#include "ladybugscreenc.c"
#include "ladybugscreenp.c"
#include "gnat1rlec.c"
#include "gnat1rlep.c"
#include "gnat2rlec.c"
#include "gnat2rlep.c"
#include "gnat3rlec.c"
#include "gnat3rlep.c"
#include "gnat4rlec.c"
#include "gnat4rlep.c"
#include "gnat5rlec.c"
#include "gnat5rlep.c"
#include "cruiserend_c.c"
#include "cruiserend_p.c"
#include "snowballbase_c.c"
#include "snowballbase_p.c"
#include "f18sprites8sl.c"
#include "f18abosses.c"

extern const unsigned int f18BlackPal[16];

void ladybugwin(void);
void soundtest(void);

void wrapenemy() {
	unsigned int old = nBank;
	SWITCH_IN_BANK14b;
	enemy();
	SWITCH_IN_PREV_BANK(old);
}

void wrapnoen(int x) {
	unsigned int old = nBank;
	SWITCH_IN_BANK14b;
	noen(x);
	SWITCH_IN_PREV_BANK(old);
}

void wrapplayer() {
	unsigned int old = nBank;
	SWITCH_IN_BANK2b;
	player();

    SWITCH_IN_BANK2a;
	if (playership != SHIP_SELENA) {
        mvshot();
    } else {
        homingshot();
    }
	SWITCH_IN_PREV_BANK(old);
}

void wrapcheat() {
	unsigned int old = nBank;
	SWITCH_IN_BANK2b;
	cheat();
	SWITCH_IN_PREV_BANK(old);
}

void wrapcolchk(int x) {
	unsigned int old = nBank;
	SWITCH_IN_BANK2a;
	colchk(x);
	SWITCH_IN_PREV_BANK(old);
}

void wrapplycol() {
	unsigned int old = nBank;
	SWITCH_IN_BANK2a;
	plycol();
	SWITCH_IN_PREV_BANK(old);
}

void wrapplayerstraight() {
	unsigned int old = nBank;
	SWITCH_IN_BANK2b;
	playerstraight();
	SWITCH_IN_PREV_BANK(old);
}
void wrapplayerleft() {
	unsigned int old = nBank;
	SWITCH_IN_BANK2b;
	playerleft();
	SWITCH_IN_PREV_BANK(old);
}
void wrapplayerright() {
	unsigned int old = nBank;
	SWITCH_IN_BANK2b;
	playerright();
	SWITCH_IN_PREV_BANK(old);
}

// boss chars run through to 255
// not used for F18A
void wrapunpackboss(unsigned int level) {
	unsigned int old = nBank;
	SWITCH_IN_BANK5a;
	// level 2 is handled at level start
	switch (level) {
		case 1:	RLEUnpack(gPATTERN+BOSS_START*8, BOSS1, (BNR*BNC)<<3); break;
		case 2:	RLEUnpack(gPATTERN+BOSS_START*8, BOSS2, (BNR*BNC)<<3); break;
		case 3:	RLEUnpack(gPATTERN+BOSS_START*8, BOSS3, (BNR*BNC)<<3); break;
		case 4:	RLEUnpack(gPATTERN+BOSS_START*8, BOSS4, (BNR*BNC)<<3); break;
		case 5:	RLEUnpack(gPATTERN+BOSS_START*8, BOSS5, (BNR*BNC)<<3); break;
	}
	SWITCH_IN_PREV_BANK(old);
}

void winwrapgetscroll(unsigned char *dst, const char *src, unsigned int cnt) {
	// copies from src (dst is usually going to be tmpbuf)
	// used to get text out of the ending scrolltext
	unsigned int old = nBank;
	SWITCH_IN_BANK8a;
	memcpy(dst, src, cnt);
	SWITCH_IN_PREV_BANK(old);
}

char winwrapgetbyte(const char *adr) {
	// another special one for win code - read one byte (so we can
	// get the termination character).
	unsigned int old = nBank;
	char ch;

	SWITCH_IN_BANK5b;
	ch = *adr;
	SWITCH_IN_PREV_BANK(old);

	return ch;
}

void wrapstars() {
	unsigned int old = nBank;
	SWITCH_IN_BANK6a;
	stars();
	SWITCH_IN_PREV_BANK(old);
}

void delaystars(unsigned int q)
{  /* delay for 'q' jiffies with star movement */
	unsigned int old = nBank;
	SWITCH_IN_BANK6a;

	while (q--)	{ 
		stars();
	}

	SWITCH_IN_PREV_BANK(old);
}

void wrapinitstars() {
	unsigned int old = nBank;
	SWITCH_IN_BANK6a;
	initstars();
	SWITCH_IN_PREV_BANK(old);
}

void wrapbaseinit() {
	unsigned int old = nBank;
	// no switch needed, we're calling the fixed bank. But we need to restore the
	// right bank before we return, because ispace changes it.
	baseinit();
	SWITCH_IN_PREV_BANK(old);
}

void wrapispace() {
	unsigned int old = nBank;
	// no switch needed, we're calling the fixed bank. But we need to restore the
	// right bank before we return, because ispace changes it.
	ispace();
	SWITCH_IN_PREV_BANK(old);
}
 
void wrapispaceraw() {
	unsigned int old = nBank;
	// no switch needed, we're calling the fixed bank. But we need to restore the
	// right bank before we return, because ispace changes it.
	ispaceraw();
	SWITCH_IN_PREV_BANK(old);
}

void wrapbackground() {
	unsigned int old = nBank;
	SWITCH_IN_BANK6a;
	background();
	SWITCH_IN_PREV_BANK(old);
}

void wrapcheckdamage(int sr, int sc, int pwr) {
	unsigned int old = nBank;
	SWITCH_IN_BANK7a;
	// warning: checkdamage returns a return value, but we are ignoring it
	checkdamage(sr, sc, pwr);
	SWITCH_IN_PREV_BANK(old);
}

void wrapLoadSelenaPic() {
	unsigned int old = nBank;
	SWITCH_IN_BANK3b;
	RLEUnpack(0x0000, selenaendP, 6144);
	RLEUnpack(0x2000, selenaendC, 6144);
	SWITCH_IN_PREV_BANK(old);
}

void wrapgetfontonebyte(unsigned char *dest, const unsigned char *src) {
	unsigned int old = nBank;
	// this explicitly gets bytes from the font as requested	
	SET_COLECO_FONT_BANK;
    *dest = *src;
	SWITCH_IN_PREV_BANK(old);
}

void wrapgetfontbytes(unsigned char *dest, const unsigned char *src, unsigned int cnt) {
	unsigned int old = nBank;
	// this explicitly gets bytes from the font as requested	
	SET_COLECO_FONT_BANK;
	memcpy(dest, src, cnt);
	SWITCH_IN_PREV_BANK(old);
}

void wrapspritescore(unsigned int sprpat, unsigned int sprtab, unsigned int row, unsigned int col, unsigned int ch) {
	unsigned int x=score;
	unsigned int old = nBank;
	unsigned char i;

	vdpmemset(sprpat,0x00,16*8);	// zero out first 4 sprites

	// copy the correct digits into the sprite patterns (skip every other pattern)
	// sprite patterns at 0x3800
	SET_COLECO_FONT_BANK;

	vdpmemcpy(sprpat, (char*)(colecofont+(((x/10000)+16)<<3)), 8);
	x%=10000;
	vdpmemcpy(sprpat+0x10, (char*)(colecofont+(((x/1000)+16)<<3)), 8);
	x%=1000;
	vdpmemcpy(sprpat+0x20, (char*)(colecofont+(((x/100)+16)<<3)), 8);
	x%=100;
	vdpmemcpy(sprpat+0x30, (char*)(colecofont+(((x/10)+16)<<3)), 8);
	vdpmemcpy(sprpat+0x40, (char*)(colecofont+(((x%10)+16)<<3)), 8);
	vdpmemcpy(sprpat+0x50, (char*)(colecofont+(16<<3)), 8);	// 0
	vdpmemcpy(sprpat+0x60, (char*)(colecofont+(scoremode+16<<3)), 8);	// scoremode

	// draw the sprites on the the screen
	VDP_SET_ADDRESS_WRITE(sprtab);		// sprite attribute table is different in bitmap mode
	for (i=0; i<4; i++) {
		VDPWD=(row);			// row
		VDP_SAFE_DELAY();
		VDPWD=(col+(i<<4));		// col
		VDP_SAFE_DELAY();
		VDPWD=(ch+(i<<2));		// pattern 
		VDP_SAFE_DELAY();
		VDPWD=(15);				// color
		VDP_SAFE_DELAY();
	}

	// zero the rest of the sprite table
	VDPWD=0xd0;

	SWITCH_IN_PREV_BANK(old);
}

void wrapLoadEngineSprites() {
	// load up the engine sprites
	unsigned int old = nBank;

    if (f18a) {
    	SWITCH_IN_BANK14a;
	    vdpmemcpy(gSPRITE_PATTERNS+76*8, F18SPRITES+76*8, 2*4*8);
	    vdpmemcpy(gSPRITE_PATTERNS+76*8+0x800, F18SPRITES2+76*8, 2*4*8);
	    vdpmemcpy(gSPRITE_PATTERNS+76*8+0x1000, F18SPRITES3+76*8, 2*4*8);
    } else {
    	SWITCH_IN_BANK5b;
	    vdpmemcpy(gSPRITE_PATTERNS+76*8, SPRITES+76*8, 2*4*8);
    }
	SWITCH_IN_PREV_BANK(old);
}

void wrapPlayerFlameBig() {
	// set the player flame sprites to big
    // we assume that the init function sets to blank for gnat and selena
	unsigned int old = nBank;

    if (playership == SHIP_GNAT) {
        vdpchar(100*8+0x0800, 0x01);						// 1 pixel on for high flame (we'll try it for F18A too)
    } else if (playership != SHIP_SELENA) {
        if (f18a) {
    	    SWITCH_IN_BANK14a;
	        vdpmemcpy(gSPRITE_PATTERNS+100*8, F18SPRITES+100*8, 4*8);
	        vdpmemcpy(gSPRITE_PATTERNS+100*8+0x800, F18SPRITES2+100*8, 4*8);
	        vdpmemcpy(gSPRITE_PATTERNS+100*8+0x1000, F18SPRITES3+100*8, 4*8);
        } else {
    	    SWITCH_IN_BANK5b;
	        vdpmemcpy(gSPRITE_PATTERNS+100*8, SPRITES+100*8, 4*8);
        }
	    SWITCH_IN_PREV_BANK(old);
    }
} 

void wrapPlayerFlameSmall() {
	// set the player flame sprites to small
	unsigned int old = nBank;

    if (playership == SHIP_GNAT) {
        vdpchar(100*8+0x0800, 0x00);						// turn the 1 pixel off for low flame
    } else if (playership != SHIP_SELENA) {
        if (f18a) {
            SWITCH_IN_BANK14a;
	        vdpmemcpy(gSPRITE_PATTERNS+100*8, F18PLAYERFLAMESMALL, 4*8);
	        vdpmemcpy(gSPRITE_PATTERNS+100*8+0x800, F18PLAYERFLAMESMALL2, 4*8);
	        vdpmemcpy(gSPRITE_PATTERNS+100*8+0x1000, F18PLAYERFLAMESMALL3, 4*8);
        } else {
    	    SWITCH_IN_BANK5b;
	        vdpmemcpy(gSPRITE_PATTERNS+100*8, PLAYERFLAMESMALL, 4*8);
        }
	    SWITCH_IN_PREV_BANK(old);
    }
} 

void wrapCopyShip(const unsigned char *p, const unsigned char *c, int idx) {
	unsigned int old = nBank;
	unsigned int i;

    // idx is 2,3,4 and tells us which ship so we can do the right bank

    if (idx == 3) {
        // split between banks
	    for (i=0; i<17; ++i) {
            SWITCH_IN_BANK10a;
		    vdpmemcpy(0x2000+6*32*8+16*8+(i*32*8), c+(i*16*8), 16*8);
            SWITCH_IN_BANK10b;
		    vdpmemcpy(0x0000+6*32*8+16*8+(i*32*8), p+(i*16*8), 16*8);
	    }
    } else {
        if (idx == 2) { SWITCH_IN_BANK10a; }
        else if (idx == 4) { SWITCH_IN_BANK10b; }
        else { SWITCH_IN_BANK9b; }

	    for (i=0; i<17; ++i) {
		    vdpmemcpy(0x2000+6*32*8+16*8+(i*32*8), c+(i*16*8), 16*8);
		    vdpmemcpy(0x0000+6*32*8+16*8+(i*32*8), p+(i*16*8), 16*8);
	    }
    }

	SWITCH_IN_PREV_BANK(old);
}

void wrapGamWin() {
	SWITCH_IN_BANK8a;
	gamwin();	// never returns
}

void wrapGameWinHard() {
    // warning: gamewinhard() doesn't run cleanexit() like the other ones do!
	unsigned int old = nBank;

    SWITCH_IN_BANK8b;
    gamewinhard();

    SWITCH_IN_PREV_BANK(old);
}

void wrapLoadStoryFont() {
	// this loads the ColecoVision ROM font like loadcharset, but instead of
	// into the pattern table, it goes into the third bitmap table for story text
	unsigned int old = nBank;

	SET_COLECO_FONT_BANK;
	// we assume pattern table is at 0
	vdpmemcpy((32*8)+4096, colecofont, 768);

	SWITCH_IN_PREV_BANK(old);
}

void wrapLoadLadyScreen() {
	unsigned int old = nBank;
	SWITCH_IN_BANK11b;
	RLEUnpack(0x0000, LADYSCREENP, 6144);
	RLEUnpack(0x2000, LADYSCREENC, 6144);
	SWITCH_IN_PREV_BANK(old);
}

void wrapldpic() {
	unsigned int old = nBank;
	SWITCH_IN_BANK9a;
	ldpic();
	SWITCH_IN_PREV_BANK(old);
}

// copies one byte over
void wrapLadyBugByte(int off) {
	if (off < 6144) {
		unsigned int old = nBank;
		SWITCH_IN_BANK11b;
		vdpchar(off, LADYBUGP[off]);
		SWITCH_IN_BANK11a;
		vdpchar(0x2000+off, LADYBUGC[off]);
		SWITCH_IN_PREV_BANK(old);
	}
}

// draw one character on a bitmap screen at off (pattern table only)
void wrapDrawLastRowText(int txt, unsigned int off) {
	unsigned int old = nBank;

	if (txt <= 32) return;

	SET_COLECO_FONT_BANK;
	txt-=32;
	vdpmemcpy(off, &colecofont[txt<<3], 8);
	SWITCH_IN_PREV_BANK(old);

}

void wraploadgnat1() {
	// needs the full screen - also needs to check music interrupts
	unsigned int old = nBank;
	SWITCH_IN_BANK12b;
	RLEUnpackInt(gnat1rlep, gnat1rlec, 6144);
	SWITCH_IN_PREV_BANK(old);
}

void wraploadgnat2() {
	// only need the top 2/3rd
	unsigned int old = nBank;
	SWITCH_IN_BANK13b;
	RLEUnpackInt(gnat2rlep, gnat2rlec, 4096);
	SWITCH_IN_PREV_BANK(old);
}

void wraploadgnat3() {
	// only need the top 2/3rd
	unsigned int old = nBank;
	SWITCH_IN_BANK13a;
	RLEUnpackInt(gnat3rlep, gnat3rlec, 4096);
	SWITCH_IN_PREV_BANK(old);
}

void wraploadgnat4() {
	// only need the top 2/3rd
	unsigned int old = nBank;
	SWITCH_IN_BANK13a;
	RLEUnpackInt(gnat4rlep, gnat4rlec, 4096);
	SWITCH_IN_PREV_BANK(old);
}

void wraploadgnat5() {
	// only need the top 2/3rd
	unsigned int old = nBank;
	SWITCH_IN_BANK5a;
	RLEUnpackInt(gnat5rlep, gnat5rlec, 4096);
	SWITCH_IN_PREV_BANK(old);
}

void wrapwarpout() {
	unsigned int old = nBank;
	SWITCH_IN_BANK8a;
    warpout();
	SWITCH_IN_PREV_BANK(old);
}

void wrapLoadFinalSnowball() {
    unsigned int old = nBank;

    if (f18a) {
        // lock it down to single color sprite mode
        VDP_SET_REGISTER(F18A_REG_ECM, 0);
    }

    SWITCH_IN_BANK5b;

    // straight ship and shields (0-15, 16-31)
	vdpmemcpy(0x3800, SNOWBALL, 8*4*8);
    // flame big (32-35)
    vdpmemcpy(0x3800+8*4*8, SPRITES+100*8, 4*8);
    // empty sprite (36-39)
    vdpmemset(0x3800+36*8, 0, 4*8);

    SWITCH_IN_PREV_BANK(old);
}

void wrapFinalSnowballBig() {
    unsigned int old = nBank;

    // F18A not used in this end sequence

	SWITCH_IN_BANK5b;

    // flame big (32-35)
    vdpmemcpy(0x3800+8*4*8, SPRITES+100*8, 4*8);

    SWITCH_IN_PREV_BANK(old);
}

void wrapFinalSnowballSmall() {
    unsigned int old = nBank;

	SWITCH_IN_BANK5b;

    // flame big (32-35)
    vdpmemcpy(0x3800+8*4*8, PLAYERFLAMESMALL, 4*8);

    SWITCH_IN_PREV_BANK(old);
}

void wrapldcruiserend() {
    unsigned int old = nBank;

    SWITCH_IN_BANK2b;
	RLEUnpack(0x2000, cruiserend_c, 6144);
    SWITCH_IN_BANK12a;
    RLEUnpack(0x0000, cruiserend_p, 6144);
    SWITCH_IN_PREV_BANK(old);
}

void wrapLoadSnowballBase() {
    unsigned int old = nBank;

    SWITCH_IN_BANK1b;
    RLEUnpackInt(SNOWBALLBASEP, SNOWBALLBASEC, 6144);
    SWITCH_IN_PREV_BANK(old);
}

const unsigned int f18bosspal1[] = {
0x0000,0x0011,0x0143,0x0161,
0x01C2,0x01A2,0x01E3,0x0281,
0x02D3,0x03F3,0x0375,0x03B3,
0x0410,0x0597,0x0681,0x0841
};
const unsigned int f18bosspal2[] = {
0x0000,0x0000,0x0111,0x0123,
0x0200,0x0234,0x0345,0x0411,
0x0467,0x0678,0x0611,0x09Ab,
0x0922,0x0b22,0x0cDd,0x0d33
};
const unsigned int f18bosspal3[] = {
0x0000,0x0125,0x013e,0x0139,
0x024c,0x025f,0x0346,0x0367,
0x037f,0x059f,0x056f,0x0558,
0x0698,0x06Bf,0x087e,0x0b9e
};
const unsigned int f18bosspal4[] = {
0x0000,0x0220,0x0222,0x0236,
0x0444,0x0540,0x0665,0x0760,
0x0788,0x0980,0x0994,0x09Ab,
0x0bA0,0x0cB0,0x0cDd,0x0eD3
};
const unsigned int f18bosspal5[] = {
0x0000,0x0000,0x0202,0x0313,
0x0424,0x0515,0x0635,0x0726,
0x0747,0x0828,0x0958,0x0939,
0x0b4a,0x0b7a,0x0d6c,0x0e9f
};


// returns 0 if finished
unsigned int wrapLoadBossF18A(unsigned int n, unsigned int scanline) {
    unsigned int old = nBank;
    unsigned char ret = 1;

    SWITCH_IN_BANK15a;

    // we load the F18A boss one scanline at a time in place of the shifting code
    // the largest boss only needs about 2k of data
    // Output width is always 128 pixels (32 bytes). Min stride is 64 bytes
    // and doing that will allow us to scroll the boss onscreen by changing the offset
    // This means the largest boss will actually take 3k in VRAM! But, we have that much. ;)

    switch (n) {
        case 1: 
            if (scanline < F18BOSS1H) {
                vdpmemcpy(scanline*32+BOSS_PATTERN, &f18bossdat1[scanline*(F18BOSS1W/4)], F18BOSS1W/4);
                vdpmemset(scanline*32+BOSS_PATTERN+(F18BOSS1W/4), 0, 32-(F18BOSS1W/4));
            } else {
	            if (scoremode == 3) {
                    // invisible enemies
                    loadpal_f18a(f18BlackPal, PAL_INVISIBLE*4, 16);
                } else {
                    loadpal_f18a(f18bosspal1, PAL_BOSS*4, 16);
                }
                ret = 0;
            }
            break;

        case 2: 
            if (scanline < F18BOSS2H) {
                vdpmemcpy(scanline*32+BOSS_PATTERN, &f18bossdat2[scanline*(F18BOSS2W/4)], F18BOSS2W/4);
                vdpmemset(scanline*32+BOSS_PATTERN+(F18BOSS2W/4), 0, 32-(F18BOSS2W/4));
            } else {
	            if (scoremode == 3) {
                    // invisible enemies
                    loadpal_f18a(f18BlackPal, PAL_INVISIBLE*4, 16);
                } else {
                    loadpal_f18a(f18bosspal2, PAL_BOSS*4, 16);
                }
                ret = 0;
            }
            break;

        case 3: 
            if (scanline < F18BOSS3H) {
                vdpmemcpy(scanline*32+BOSS_PATTERN, &f18bossdat3[scanline*(F18BOSS3W/4)], F18BOSS3W/4);
                vdpmemset(scanline*32+BOSS_PATTERN+(F18BOSS3W/4), 0, 32-(F18BOSS3W/4));
            } else {
	            if (scoremode == 3) {
                    // invisible enemies
                    loadpal_f18a(f18BlackPal, PAL_INVISIBLE*4, 16);
                } else {
                    loadpal_f18a(f18bosspal3, PAL_BOSS*4, 16);
                }
                ret = 0;
            }
            break;

        case 4: 
            if (scanline < F18BOSS4H) {
                vdpmemcpy(scanline*32+BOSS_PATTERN, &f18bossdat4[scanline*(F18BOSS4W/4)], F18BOSS4W/4);
                vdpmemset(scanline*32+BOSS_PATTERN+(F18BOSS4W/4), 0, 32-(F18BOSS4W/4));
            } else {
	            if (scoremode == 3) {
                    // invisible enemies
                    loadpal_f18a(f18BlackPal, PAL_INVISIBLE*4, 16);
                } else {
                    loadpal_f18a(f18bosspal4, PAL_BOSS*4, 16);
                }
                ret = 0;
            }
            break;

        case 5: 
            if (scanline < F18BOSS5H) {
                vdpmemcpy(scanline*32+BOSS_PATTERN, &f18bossdat5[scanline*(F18BOSS5W/4)], F18BOSS5W/4);
                vdpmemset(scanline*32+BOSS_PATTERN+(F18BOSS5W/4), 0, 32-(F18BOSS5W/4));
            } else {
	            if (scoremode == 3) {
                    // invisible enemies
                    loadpal_f18a(f18BlackPal, PAL_INVISIBLE*4, 16);
                } else {
                    loadpal_f18a(f18bosspal5, PAL_BOSS*4, 16);
                }
                ret = 0;
            }
            break;
    }

    SWITCH_IN_PREV_BANK(old);
    return ret;
}

void wrapLoadF18MainPalette() {
    unsigned int old = nBank;

    SWITCH_IN_BANK14a;
    loadpal_f18a(F18PALETTE, 0, 56);    // load all but the player ship palette
    SWITCH_IN_PREV_BANK(old);
}

void wrapInitCruiser() {
	unsigned int old = nBank;

    if (f18a) {
        SWITCH_IN_BANK14a;
        initCruiserf18();
    } else {
	    SWITCH_IN_BANK5b;
	    vdpmemcpy(108*8+0x0800, &SPRITES[108*8], 24*4*8);	// ship sprites
    }

    SWITCH_IN_BANK2b;
    initCruiserBase();

    SWITCH_IN_PREV_BANK(old);
}

void wrapInitSnowball() {
	unsigned int old = nBank;

    if (f18a) {
        SWITCH_IN_BANK13b;
        initSnowballf18();
    } else {
    	SWITCH_IN_BANK5b;
    	vdpmemcpy(108*8+0x0800, SNOWBALL, 24*4*8);			// ship sprites
    }

    SWITCH_IN_BANK2b;
    initSnowballBase();
    
    SWITCH_IN_PREV_BANK(old);
}

void wrapInitLadybug() {
	unsigned int old = nBank;

    if (f18a) {
	    SWITCH_IN_BANK9b;
        initLadybugf18();
    } else {
	    SWITCH_IN_BANK5b;
	    vdpmemcpy(108*8+0x0800, LADYBUG, 24*4*8);			// ship sprites
    }

    SWITCH_IN_BANK2b;
    initLadybugBase();

	SWITCH_IN_PREV_BANK(old);
}

void wrapInitGnat() {
	unsigned int old = nBank;

    if (f18a) {
	    SWITCH_IN_BANK15b;
        initGnatf18();
    } else {
	    SWITCH_IN_BANK5b;
	    vdpmemcpy(108*8+0x0800, GNAT, 24*4*8);				// ship sprites
    }

    SWITCH_IN_BANK2b;
    initGnatBase();

    SWITCH_IN_PREV_BANK(old);
}

void wrapInitSelena() {
	unsigned int old = nBank;

    if (f18a) {
	    SWITCH_IN_BANK15b;
        initSelenaf18();
    } else {
	    SWITCH_IN_BANK5b;
	    vdpmemcpy(108*8+0x0800, SELENA, 24*4*8);			// ship sprites
        vdpmemcpy(96*8+0x0800, HOMING, 4*8);                // homing shot
    }

    SWITCH_IN_BANK2b;
    initSelenaBase();

	SWITCH_IN_PREV_BANK(old);
}

void wrapLoadBossGfx() {
	unsigned int old = nBank;

    SWITCH_IN_BANK8a;
    loadBossGfx();
	SWITCH_IN_PREV_BANK(old);
}

void wrapRestoreBossGfx() {
	unsigned int old = nBank;

    SWITCH_IN_BANK8a;
    restoreBossGfx();
	SWITCH_IN_PREV_BANK(old);
}

void wrapAddDestroyed(unsigned int ptr) {
	unsigned int old = nBank;

    SWITCH_IN_BANK8a;
    AddDestroyed(ptr);
	SWITCH_IN_PREV_BANK(old);
}

void wrapbossdraw() {
	unsigned int old = nBank;

    SWITCH_IN_BANK7b;

    switch(level) {
        case 1: 
            draw1();
            break;
        case 2: 
            draw2();
            break;
        case 3: 
            draw3();
            break;
        case 4: 
            draw4();
            break;
        case 5: 
            draw5();
            break;
    }

	SWITCH_IN_PREV_BANK(old);
}

void wrapladybugwin() {
	unsigned int old = nBank;

    SWITCH_IN_BANK14b;
    ladybugwin();
	SWITCH_IN_PREV_BANK(old);
}

void wrapsoundtest() {
	unsigned int old = nBank;

    SWITCH_IN_BANK3b;
    soundtest();
	SWITCH_IN_PREV_BANK(old);
}

void wrapRegisterHiScore() {
	unsigned int old = nBank;

    SWITCH_IN_BANK8a;
    registerHiScore();  // warning: might reboot instead of returning
	SWITCH_IN_PREV_BANK(old);
}

int wrapCheckHighScores() {
	unsigned int old = nBank;
    int ret;

    SWITCH_IN_BANK8a;
    ret = checkHighScores();
	SWITCH_IN_PREV_BANK(old);

    return ret;
}

void wrapClearHighScores() {
	unsigned int old = nBank;

    SWITCH_IN_BANK8a;
    clearHighScores();
	SWITCH_IN_PREV_BANK(old);
}

void wrapSaveScores(struct _scores *scores) {
	unsigned int old = nBank;

    SWITCH_IN_BANK8a;
    saveScores(scores);
	SWITCH_IN_PREV_BANK(old);
}

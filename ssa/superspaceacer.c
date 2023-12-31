/* program SUPER SPACE ACER design version 2.2 */
/* ported to ColecoVision by M.Brent */

// libti99
#include <vdp.h>
#include <f18a.h>
#include <sound.h>
#include <kscan.h>

#include <TISNPlay.h>

// game
#include "game.h"
#include "enemy.h"
#include "trampoline.h"
#include "human.h"
#include "music.h"
#include "attract.h"
#include "boss.h"
#include "f18load.h"

#define BIN2INC_HEADER_ONLY
#include "f18sprites8sl.c"
#include "harmlesslion_c.c"
#include "harmlesslion_p.c"

// full software reboot vector (warning: hard coded but defined by the crt0)
static void (* const hwreboot)()=(void (*const)())0x602a;

// startup and init, central code
#define SIZE_OF_CHARS		176
#define SIZE_OF_SPRITES		2048
#define LIVESCHARSSA		19
#define LIVESCHARLADYBUG	20
#define LIVESCHARGNAT		21

// for sprite flicker, manage in CPU memory, copy up during vblank
struct _sprite SpriteTab[32];

// player data for weapons, but used in multiple places
const unsigned int damage[8] = { 3,4,5,1, 2,2,3,1 };

// distns table per stage (level-1)
const int stage_distns[5] = { 1100, 1150, 1200, 1400, 1650 };

// multi-purpose
int a,b,x,y;
int flag;

// cheat flag
int ch;

// which ship did the player choose?
unsigned int playership = 255;
// how many pixels down does the sprite start? (for smaller ones)
unsigned int playerOffset;
// how far down to draw the flame? (was the same before the F18A sprites)
unsigned int flameOffset;
// old shield color - used to detect changes so we don't upload the graphics every frame
int oldshield = 0;
// force bonus - if you never shoot except during the boss, and never miss, you get it
unsigned int force = 0;

// holds whether the F18A was detected
int f18a = 0;

// function pointers for shield graphics swap functions
// these functions MUST exist in the fixed bank
void (*shieldsOn)() = shieldCruiser;
void (*shieldsOff)() = deShieldCruiser;

// stage and game related
unsigned int level;
int distns;
unsigned int nDifficulty;
unsigned int seed=1;
unsigned char attractShip = 0;
unsigned int bgColor = COLOR_BLACK;

extern const unsigned char CHARS[];
extern const unsigned char ELECTRICWALL[];
extern unsigned char screenFlashCnt;

// used for patcpy and scroll and F18a boss explosion
unsigned char tmpbuf[64];

// used for bank switch returns
unsigned int nBank = 0xffff;

// used for the cloaked enemy mode!
const unsigned int f18BlackPal[16] = { 0,0,0,0,0,0,0,0,
                                       0,0,0,0,0,0,0,0 };

void sprStart0() {
	// copy sprites 0-31 in order (simplest case)
	__asm__ (
        "li r0,SpriteTab+0\n\t"
        "li r1,0x8C00\n\t"
        ".rept 128\n\t"
        "movb *r0+,*r1\n\t"
        ".endr\n\t"
        : : : "r0","r1"
    );
}

void sprStart4() {
	// copy sprites 4-31, then 0-3
	// decrement mode
	__asm__ (
        "li r0,SpriteTab+124\n\t"
        "li r1,0x8C00\n\t"
        "li r2,-8\n\t"
        ".rept 28\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "a r2,r1\n\t"
        ".endr\n\t"
        
        "li r0,SpriteTab+12\n\t"
        ".rept 4\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "a r2,r1\n\t"
        ".endr\n\t"
        
        : : : "r0","r1","r2"
    );
}

void sprStart8() {
	// copy sprites 8-31, then 0-7
	__asm__ (
        "li r0,SpriteTab+32\n\t"
        "li r1,0x8C00\n\t"
        ".rept 96\n\t"
        "movb *r0+,*r1\n\t"
        ".endr\n\t"
        
        "li r0,SpriteTab+0\n\t"
        ".rept 32\n\t"
        "movb *r0+,*r1\n\t"
        ".endr\n\t"
        
        : : : "r0","r1"
    );
}

void sprStart12() {
	// copy sprites 12-31, then 0-11
	// decrement mode
	__asm__ (
        "li r0,SpriteTab+124\n\t"
        "li r1,0x8C00\n\t"
        "li r2,-8\n\t"
        ".rept 20\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "a r2,r1\n\t"
        ".endr\n\t"
        
        "li r0,SpriteTab+44\n\t"
        ".rept 12\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "a r2,r1\n\t"
        ".endr\n\t"
        
        : : : "r0","r1","r2"
    );
}

void sprStart16() {
	// copy sprites 16-31, then 0-15
	__asm__ (
        "li r0,SpriteTab+64\n\t"
        "li r1,0x8C00\n\t"
        ".rept 64\n\t"
        "movb *r0+,*r1\n\t"
        ".endr\n\t"
        
        "li r0,SpriteTab+0\n\t"
        ".rept 64\n\t"
        "movb *r0+,*r1\n\t"
        ".endr\n\t"
        
        : : : "r0","r1"
    );
}

void sprStart20() {
	// copy sprites 20-31, then 0-19
	// decrement mode
	__asm__ (
        "li r0,SpriteTab+124\n\t"
        "li r1,0x8C00\n\t"
        "li r2,-8\n\t"
        ".rept 12\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "a r2,r1\n\t"
        ".endr\n\t"
        
        "li r0,SpriteTab+76\n\t"
        ".rept 20\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "a r2,r1\n\t"
        ".endr\n\t"
        
        : : : "r0","r1","r2"
    );
}

void sprStart24() {
	// copy sprites 24-31, then 0-23
	__asm__ (
        "li r0,SpriteTab+96\n\t"
        "li r1,0x8C00\n\t"
        ".rept 32\n\t"
        "movb *r0+,*r1\n\t"
        ".endr\n\t"
        
        "li r0,SpriteTab+0\n\t"
        ".rept 96\n\t"
        "movb *r0+,*r1\n\t"
        ".endr\n\t"
        
        : : : "r0","r1"
    );
}

void sprStart28() {
	// copy sprites 28-31, then 0-27
	// decrement mode
	__asm__ (
        "li r0,SpriteTab+112\n\t"
        "li r1,0x8C00\n\t"
        "li r2,-8\n\t"
        ".rept 4\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "a r2,r1\n\t"
        ".endr\n\t"
        
        "li r0,SpriteTab+108\n\t"
        ".rept 28\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "movb *r0+,*r1\n\t"
        "a r2,r1\n\t"
        ".endr\n\t"
        
        : : : "r0","r1","r2"
    );
}

int checkLimi() {
    int ret;
    
    __asm__ (
        "clr r12\n\tseto %0\n\ttb 2\n\tjne 2\n\tclr %0\n\tnop" 
        : "=rm"(ret) : : "r12" 
    );
  
    return ret;
}

void musicsync() {
	// safely check if it's time to play music
	if (checkLimi()) {
		VDP_INT_POLL;
		doMusic();
	}
}

// end of frame handler - handles sprite copy unless we were late
void waitforstep() {
	static unsigned char nSpriteOffset=0;

	// wait for one to occur
	// note, if we were late, this will just trigger
	// that makes the fast table copy unsafe. So, we
	// test a return value to know if it was real or late.
	if (vdpwaitvint()) {
		// missed - resync with the VDP rather than not updating sprites
		doMusic();		// harder to see it if you don't hear it ;)
		if (vdpwaitvint()) {
			return;		// it was late twice? That should not happen!
		}
	}

	// blit up the sprite table real quick - we should still be in blank
	VDP_SET_ADDRESS_WRITE(gSPRITES);

	// there are 8 possible sprite rotations, select the correct one
	// each function is a fully unrolled OUTI loop for the particular pattern
	// to get the sprite table to the VDP ASAP. Takes a bit over 1k for all that.
	// Note if nSpriteOffset is ever any other number, then we get no sprite updates
	// for that frame!! Be careful!
	switch (nSpriteOffset&0x07) {
	case 0:
		sprStart0();
		break;
	case 1:
		sprStart4();
		break;
	case 2:
		sprStart8();
		break;
	case 3:
		sprStart12();
		break;
	case 4:
		sprStart16();
		break;
	case 5:
		sprStart20();
		break;
	case 6:
		sprStart24();
		break;
	case 7:
		sprStart28();
		break;
	}

	// update sprite offset
	++nSpriteOffset;

	// play music
	doMusic();  
}

// loads a default ASCII character set from ROM
void loadcharset() {
	// this loads the ColecoVision ROM font - note we copy it into all tables
	// it was shifted and OR'd to make it look fatter - idea from Sometimes99er!
	unsigned int old = nBank;

	SET_COLECO_FONT_BANK;

    vdpmemcpy(gPATTERN+(32*8), colecofont, 768);
    if (!f18a) {
	    vdpmemcpy(gPATTERN+(32*8)+SCROLL_OFFSET, colecofont, 768);
	    vdpmemcpy(gPATTERN+(32*8)+(SCROLL_OFFSET*2), colecofont, 768);
	    vdpmemcpy(gPATTERN+(32*8)+(SCROLL_OFFSET*3), colecofont, 768);
    }

	// because 'y' and 'z' are overwritten by boss chars and color,
	// we move them down to '[' and '\' respectively
	patcpy('y', '[');
	patcpy('z', '\\');

	SWITCH_IN_PREV_BANK(old);
}

// copies a VDP character pattern to all four tables
// if from is 0, assumes that tmpbuf is already loaded
void patcpy(int from, int to) {
	if (from) {
		vdpmemread(((int)from<<3)+gPATTERN, tmpbuf, 8);
	}
	vdpmemcpy(((int)to<<3)+gPATTERN, tmpbuf, 8);
    if (!f18a) {
	    vdpmemcpy(((int)to<<3)+gPATTERN+SCROLL_OFFSET, tmpbuf, 8);
	    vdpmemcpy(((int)to<<3)+gPATTERN+(SCROLL_OFFSET*2), tmpbuf, 8);
	    vdpmemcpy(((int)to<<3)+gPATTERN+(SCROLL_OFFSET*3), tmpbuf, 8);
    }
}

// copies a VDP character pattern to sprite table
void patsprcpy(int from, int to) {
	vdpmemread((from<<3)+gPATTERN, tmpbuf, 8);
	vdpmemcpy((to<<3)+gSPRITE_PATTERNS, tmpbuf, 8);
}

void spdall() {
	unsigned char i;
	for (i=0; i<32; i++) {
		spdel(i);
	}
}

// warning: this seed is reset by the level code, so this game is
// intentionally not very random.
unsigned char rndnum()
{
	// these are all the masks
#if 0
/* 00 */       0x00,  //             0
/* 01 */       0x01,  //             1
/* 02 */       0x03,  //             3
/* 03 */       0x06,  //             7
/* 04 */       0x0C,  //            15
/* 05 */       0x14,  //            31
/* 06 */       0x30,  //            63
/* 07 */       0x60,  //           127
/* 08 */       0xB8,  //           255
/* 09 */     0x0110,  //           511
/* 10 */     0x0240,  //         1,023
/* 11 */     0x0500,  //         2,047
/* 12 */     0x0CA0,  //         4,095
/* 13 */     0x1B00,  //         8,191
/* 14 */     0x3500,  //        16,383
/* 15 */     0x6000,  //        32,767
/* 16 */     0xB400,  //        65,535
/* 17 */ 0x00012000,  //       131,071
/* 18 */ 0x00020400,  //       262,143
/* 19 */ 0x00072000,  //       524,287
/* 20 */ 0x00090000,  //     1,048,575
/* 21 */ 0x00140000,  //     2,097,151
/* 22 */ 0x00300000,  //     4,194,303
/* 23 */ 0x00400000,  //     8,388,607
/* 24 */ 0x00D80000,  //    16,777,215
/* 25 */ 0x01200000,  //    33,554,431
/* 26 */ 0x03880000,  //    67,108,863
/* 27 */ 0x07200000,  //   134,217,727
/* 28 */ 0x09000000,  //   268,435,575
/* 29 */ 0x14000000,  //   536,870,911
/* 30 */ 0x32800000,  // 1,073,741,823
/* 31 */ 0x48000000,  // 2,147,483,647
/* 32 */ 0xA3000000   // 4,294,967,295
#endif

	// trying the dreamcast one again, but 8-bit this time
	if (seed&1) {
		seed >>= 1;
		seed ^= 0xb8;
	} else {
		seed >>= 1;
	}

	return seed;
}

void cls()
{ /* 'clear' screen with 32 */
	vdpmemset(gIMAGE, 32, 768);
}

// leave this one in the fixed bank
unsigned char intpic() {
	unsigned char i;

	// set the mode (returns disabled and uninitialized)
	i = set_bitmap_raw(VDP_SPR_16x16);
	// one difference
	VDP_SET_REGISTER(VDP_REG_SDT, 0x07);	gSpritePat = 0x3800;

    if (f18a) {
        // turn off the extended sprite mode
        VDP_SET_REGISTER(F18A_REG_ECM, 0);
    }

    // we do use the variables here, not the macros

	// disable sprite table
	vdpmemset(gSprite, 0xd0, 128);

	// fill the image table with three sets of 0-255
	vdpwriteinc(gImage, 0, 768);

	// return the magic value
	return i;
}

void RLEUnpack(unsigned int p, const unsigned char *buf, unsigned int nMax) {
	unsigned char z;
	int cnt;	// looks like the boss pack code has some bugs and packs too many bytes, we need this

	cnt = nMax;
	VDP_SET_ADDRESS_WRITE(p);
	while (cnt > 0) {
		z=*buf;
		if (z&0x80) {
			// run of byte
			buf++;
			z&=0x7f;
			raw_vdpmemset(*buf, z);
			buf++;
		} else {
			// sequence of data
			buf++;
			raw_vdpmemcpy(buf, z);
			buf+=z;
		}
		cnt-=z;
	}
}

// does an interleaved unpack of picture and color, with music interrupts allowed
// this won't be perfect, but it'll be close
// Unlike the main unpack function, this one allows you to request fewer bytes
// than the original file was packed for... this slows it a bit.
void RLEUnpackInt(const unsigned char *bufp, const unsigned char *bufc, unsigned int nMax) {
	unsigned char z;
	unsigned int pAdr = 0;
	unsigned int cAdr = 0x2000;
	int pCnt, cCnt;		// keep these signed in case of pack bugs

	pCnt = nMax;
	cCnt = nMax;

	// we'll try to keep the two tables relatively close
	while ((pCnt>0)||(cCnt>0)) {
		// color
		while ((cCnt > 0)&&(cAdr <= pAdr+0x2000)) {
			z=*(bufc++);
			if (z&0x80) {
				// run of bytes
				z&=0x7f;
				if (z>cCnt) z=cCnt;
				vdpmemset(cAdr, *(bufc++), z);
				cAdr+=z;
			} else {
				// sequence of data
				if (z>cCnt) z=cCnt;
				vdpmemcpy(cAdr, bufc, z);
				bufc+=z;
				cAdr+=z;
			}
			cCnt-=z;

			// check for music
			musicsync();
		}

		// pattern
		while ((pCnt > 0)&&(cAdr-0x2000 >= pAdr)) {
			z=*(bufp++);
			if (z&0x80) {
				// run of bytes
				z&=0x7f;
				if (z > pCnt) z=pCnt;
				vdpmemset(pAdr, *(bufp++), z);
				pAdr+=z;
			} else {
				// sequence of data
				if (z > pCnt) z=pCnt;
				vdpmemcpy(pAdr, bufp, z);
				bufp+=z;
				pAdr+=z;
			}
			pCnt-=z;

			// check for music
			musicsync();
		}

	}
}

void noen(int x) { 
	/* remove enemy x from service */
	spdel(x+ENEMY_SPRITE);
	ent[x]=ENEMY_NONE;
}

int target(unsigned int dest, unsigned int src)
{ 
	// similar to sgn(), but designed to work with unsigned values
	// without wraparound
	if (dest > src) return 1;
	if (dest < src) return -1;
	return 0;
}

unsigned char grf1() {
	int x;

	x = set_graphics_raw(VDP_SPR_16x16);
	// we separate sprites and patterns
    // f18a uses one pattern table at 0x2000. Non-F18 has 4 tables for boss scrolling (2000,2800,3000,3800)
    // However, F18A has 3 sprite descriptor tables at 0800,1000,1800 (to get 8 colors)
    if (f18a) {
        VDP_SET_REGISTER(F18A_REG_ECM, 3);  // sprites have 3 bitplanes
    }
   	VDP_SET_REGISTER(VDP_REG_PDT, 0x04);
    gPattern = 0x2000;  // set it, but we usually use the macro version

	// clear the screen
	cls();

	// clear the sprite attribute table
	spdall();

	// return the magic word
	return x;
}

void DelSprButPlayer(unsigned int x) {
	// delete all sprites but main player sprites, and 'x'ception
	// warning: deletes shield sprites too
	unsigned int i;

	for (i=0; i<32; i++) {
		if (i<PLAYER_SPRITE+4) continue;
		if (i==x) continue;
		spdel(i);
	}
}

void shieldCruiser() {
	unsigned int old = nBank;
	SWITCH_IN_BANK5b;
	vdpmemcpy(124*8+0x0800, ALTSHIELDS, 4*4*8);			// straight
	vdpmemcpy(156*8+0x0800, &ALTSHIELDS[4*4*8], 4*4*8);	// left
	vdpmemcpy(188*8+0x0800, &ALTSHIELDS[8*4*8], 4*4*8);	// right
    SWITCH_IN_PREV_BANK(old);
}

void shieldSnowball() {
	unsigned int old = nBank;
	SWITCH_IN_BANK5b;
	vdpmemcpy(124*8+0x0800, ALTSNOWBALL, 4*4*8);			// straight
	vdpmemcpy(156*8+0x0800, &ALTSNOWBALL[4*4*8], 4*4*8);	// left
	vdpmemcpy(188*8+0x0800, &ALTSNOWBALL[8*4*8], 4*4*8);	// right
	SWITCH_IN_PREV_BANK(old);
}

void shieldLadybug() {
	unsigned int old = nBank;
	SWITCH_IN_BANK5b;
	vdpmemcpy(124*8+0x0800, ALTLADYBUG, 4*4*8);			// straight
	vdpmemcpy(156*8+0x0800, &ALTLADYBUG[4*4*8], 4*4*8);	// left
	vdpmemcpy(188*8+0x0800, &ALTLADYBUG[8*4*8], 4*4*8);	// right
	SWITCH_IN_PREV_BANK(old);
}

void shieldGnat() {
	unsigned int old = nBank;
	SWITCH_IN_BANK5b;
	vdpmemcpy(124*8+0x0800, ALTGNAT, 4*4*8);			// straight
	vdpmemcpy(156*8+0x0800, &ALTGNAT[4*4*8], 4*4*8);	// left
	vdpmemcpy(188*8+0x0800, &ALTGNAT[8*4*8], 4*4*8);	// right
	SWITCH_IN_PREV_BANK(old);
}

void shieldSelena() {
	unsigned int old = nBank;
	SWITCH_IN_BANK5b;
	vdpmemcpy(124*8+0x0800, ALTSELENA, 4*4*8);			// straight
	vdpmemcpy(156*8+0x0800, &ALTSELENA[4*4*8], 4*4*8);	// left
	vdpmemcpy(188*8+0x0800, &ALTSELENA[8*4*8], 4*4*8);	// right
	SWITCH_IN_PREV_BANK(old);
}

void deShieldCruiser() {
	unsigned int old = nBank;
	SWITCH_IN_BANK5b;
	vdpmemcpy(124*8+0x0800, &SPRITES[124*8], 4*4*8);	// straight
	vdpmemcpy(156*8+0x0800, &SPRITES[156*8], 4*4*8);	// left
	vdpmemcpy(188*8+0x0800, &SPRITES[188*8], 4*4*8);	// right
    SWITCH_IN_PREV_BANK(old);
}

void deShieldSnowball() {
	unsigned int old = nBank;
	SWITCH_IN_BANK5b;
	vdpmemcpy(124*8+0x0800, &SNOWBALL[16*8], 4*4*8);	// straight
	vdpmemcpy(156*8+0x0800, &SNOWBALL[48*8], 4*4*8);	// left
	vdpmemcpy(188*8+0x0800, &SNOWBALL[80*8], 4*4*8);	// right
	SWITCH_IN_PREV_BANK(old);
}

void deShieldLadybug() {
	unsigned int old = nBank;
	SWITCH_IN_BANK5b;
	vdpmemcpy(124*8+0x0800, &LADYBUG[16*8], 4*4*8);	// straight
	vdpmemcpy(156*8+0x0800, &LADYBUG[48*8], 4*4*8);	// left
	vdpmemcpy(188*8+0x0800, &LADYBUG[80*8], 4*4*8);	// right
	SWITCH_IN_PREV_BANK(old);
}

void deShieldGnat() {
	unsigned int old = nBank;
	SWITCH_IN_BANK5b;
	vdpmemcpy(124*8+0x0800, &GNAT[16*8], 4*4*8);	// straight
	vdpmemcpy(156*8+0x0800, &GNAT[48*8], 4*4*8);	// left
	vdpmemcpy(188*8+0x0800, &GNAT[80*8], 4*4*8);	// right
	SWITCH_IN_PREV_BANK(old);
}

void deShieldSelena() {
	unsigned int old = nBank;
	SWITCH_IN_BANK5b;
	vdpmemcpy(124*8+0x0800, &SELENA[16*8], 4*4*8);	// straight
	vdpmemcpy(156*8+0x0800, &SELENA[48*8], 4*4*8);	// left
	vdpmemcpy(188*8+0x0800, &SELENA[80*8], 4*4*8);	// right
	SWITCH_IN_PREV_BANK(old);
}

// for the F18 we can just turn the sprites on and off
void shieldf18() {
	sprite(PLAYER_SHIELD,124,PAL_PLAYSHIELD,192,112);
	sprite(PLAYER_SHIELD+1,128,PAL_PLAYSHIELD,192,128);
	sprite(PLAYER_SHIELD+2,132,PAL_PLAYSHIELD,208,112);
	sprite(PLAYER_SHIELD+3,136,PAL_PLAYSHIELD,208,128);
}
void deshieldf18() {
    spdel(PLAYER_SHIELD);
    spdel(PLAYER_SHIELD+1);
    spdel(PLAYER_SHIELD+2);
    spdel(PLAYER_SHIELD+3);
}

void displayHLLogo() {
	unsigned char i;

	shutup();

	i=intpic();
	
	SWITCH_IN_BANK12b;
	RLEUnpack(0x0000, hl_logoP, 6144);
	SWITCH_IN_BANK15b;
	RLEUnpack(0x2000, hl_logoC, 6144);

	spdall();
	// enable the screen
	VDP_SET_REGISTER(VDP_REG_MODE1, i);
	FIX_KSCAN(i);

    for (i=0; i<180; ++i) {
        VDP_WAIT_VBLANK_CRU;
        VDP_CLEAR_VBLANK;
        kscanfast(1);
        if (KSCAN_KEY == JOY_FIRE) break;
        kscanfast(2);
        if (KSCAN_KEY == JOY_FIRE) break;
    }
}

extern void draw1();
extern char br,bc,bd;

extern void selenawin();
void main() {
	unsigned char i;

    // Turn off most of the console interrupt routine
    VDP_INT_CTRL = VDP_INT_CTRL_DISABLE_SPRITES|VDP_INT_CTRL_DISABLE_SOUND;

	// check for a score saved off above stack. If it's there, we can load it and also zero playership
	memcpy(tmpbuf, SAVEDSCORE, 4);
	score = *((unsigned short*)tmpbuf);
	if (~score == *((unsigned short*)(&tmpbuf[2]))) {
		// it matches!
		playership = 0;
		scoremode = *SAVEDMODE;
		seed = (scoremode>>4)&0x0f;
		scoremode &= 0x0f;
		attractShip = *SAVEDATTRACT;
		*SAVEDATTRACT = attractShip & 1;
		attractShip >>= 4;
        f18a=*SAVEDF18A;
	} else {
		// it was junk
		score = 0;
		scoremode = 0;

        f18a = detect_f18a();
        if (f18a) {
            // check keypad for 0 - this will disable F18A
            kscanfast(1);
            if (KSCAN_KEY == '0') {
                // enable flicker before we lock the F18A
                VDP_SET_REGISTER(F18A_REG_MAXSPR, 4);
                lock_f18a();
                f18a=0;
            }
        }

        // since it's the first run, do the HL logo
        displayHLLogo();
	}
	if (seed == 0) ++seed;

    if (f18a) {
        // TODO: this needs the graphics in 14a - maybe we can copy them to the top of the 32k if we can get fixed down?
        // Otherwise, it's one time, it's okay if it's a bit slower
        SWITCH_IN_BANK7b;
        initF18GPU();                           // and load the GPU code and init the graphics
    }

    initSound();

titleagain:
	screen(COLOR_BLACK);

    if (f18a) {
        // load the palette we plan to use
        wrapLoadF18MainPalette();
    }

	SWITCH_IN_BANK9a;
	handleTitlePage();

	if (joynum == 0) {
		// this is an attract timeout
		// we'll use a flag in VDP above the score to note
		// gameplay attract or story attract
		if (!(*SAVEDATTRACT)) {
			*SAVEDATTRACT = 1;
			SWITCH_IN_BANK12a;
			doAttract();	// never returns
		} else {
			// we'll do the demo play
			*SAVEDATTRACT = 0;
		}
	}

	// set regular graphics mode
	i = grf1();
	
	/*load VDP data */
	loadcharset();
	
	SWITCH_IN_BANK5b;
	vdpmemcpy(gPATTERN, CHARS, SIZE_OF_CHARS);
    if (!f18a) {
    	vdpmemcpy(gPATTERN+SCROLL_OFFSET, CHARS, SIZE_OF_CHARS);
    	vdpmemcpy(gPATTERN+(SCROLL_OFFSET*2), CHARS, SIZE_OF_CHARS);
    	vdpmemcpy(gPATTERN+(SCROLL_OFFSET*3), CHARS, SIZE_OF_CHARS);
    }

	spdall();	// clears sprite table
	vdpmemset(gSPRITES, 0xd0, 128);	// clears VDP copy of sprite table (fixes initial gfx glitch)
	VDP_SET_REGISTER(VDP_REG_MODE1, i);	// Switch screen on
	FIX_KSCAN(i);

	sgrint();	// sets color table

	SWITCH_IN_BANK4a;
	scoremode = 0;
	if (joynum != 0) {
        playership = 0;     // always reset to cruiser on entry
		getDifficulty();	// sets scoremode
	} else {
		// demo mode
		nDifficulty = DIFFICULTY_EASY;
		playership = attractShip++;
		if (attractShip > 4) attractShip=0;
		oldscore = score;
	}
	cls();
	spdall();
	// set background to black
	screen(COLOR_BLACK);
    waitforstep();

	// load in the sprites (patterns were modified by getDifficulty)
    if (f18a) {
        SWITCH_IN_BANK14a;
	    vdpmemcpy(gSPRITE_PATTERNS, F18SPRITES, SIZE_OF_SPRITES);
	    vdpmemcpy(gSPRITE_PATTERNS+0x800, F18SPRITES2, SIZE_OF_SPRITES);
	    vdpmemcpy(gSPRITE_PATTERNS+0x1000, F18SPRITES3, SIZE_OF_SPRITES);
	    // also, manually clear out sprite patterns 248-251 (powerup,etc)
	    vdpmemset(gSPRITE_PATTERNS+(248*8), 0, 32);
	    vdpmemset(gSPRITE_PATTERNS+(248*8)+0x800, 0, 32);
    } else {
    	SWITCH_IN_BANK5b;
	    vdpmemcpy(gSPRITE_PATTERNS, SPRITES, SIZE_OF_SPRITES);
	    // also, manually clear out sprite patterns 248-251 (powerup,etc)
	    vdpmemset(gSPRITE_PATTERNS+(248*8), 0, 32);
    }
	spdall();	// clears sprite table
	vdpmemset(gSPRITES, 0xd0, 128);	// clears VDP copy of sprite table (fixes initial gfx glitch)

    // flameoffset is 32 for all but a few F18A sprites, overridden below
    flameOffset = 32;
    
    // load the correct ship
	switch (playership) {
	case SHIP_CRUISER:
		wrapInitCruiser();
		break;
	case SHIP_SNOWBALL:
		wrapInitSnowball();
		break;
	case SHIP_LADYBUG:
		wrapInitLadybug();
		break;
	case SHIP_GNAT:
		wrapInitGnat();
		break;
	case SHIP_SELENA:
		wrapInitSelena();
		break;
	}
    if (f18a) {
        // these are fixed on F18A
        playerColor = PAL_PLAYSHIP;
	    shieldsOn = shieldf18;
	    shieldsOff = deshieldf18;

        if (scoremode == 3) {
            // enemies are going to be bgcolor -- which is always black so we can assume
            // going to fix this by setting the palettes appropriately
            loadpal_f18a(f18BlackPal, PAL_INVISIBLE*4, 8);
        }
    }

	while (1) {		/*forever*/
		score=0;
		lives=3;
		level=1;
		if (playership != SHIP_GNAT) {
			pwrlvl=PWRPULSE; 
            oldpwrlvl = PWR3WAY;
			if (playership == SHIP_SELENA) {
				pwrlvl+=2;
				lives = 0;
			} else if (playership == SHIP_CRUISER) {
				lives = 0;
				shield = 75;
			}
		} else {
			pwrlvl=PWRGNAT;		// this was originally a bug in cheat mode, but I like it!!
		}
		if (joynum == 0) {
			lives = 0;
            level = rndnum()%5+1;
		}

		distns=stage_distns[0];
        if (nDifficulty == DIFFICULTY_EASY) distns-=300;
        if (nDifficulty == DIFFICULTY_MEDIUM) distns-=150;
		if (joynum == 0) {
			// start at an offset to make attract mode a little more random
			distns -= rndnum();
			distns -= rndnum();
		}

		while (level<6)
		{
            force = 1;  // force is okay
			space();
			if (flag != PLAYER_DIED) {
				// means game over
				SWITCH_IN_BANK7a;
                if (force) force = 2;
				boss();
			}
			// have to check again in case you died facing a boss
			if (joynum) {
				if (flag == PLAYER_DIED) {
					// 2 means game over
					gamovr();
				}
			} else {
				// back to title screen
				level = 9;
			}

			// 3 means died during boss battle
			// the distns was already rolled back, so we can do nothing and just loop
			if ((flag != PLAYER_DIED) && (flag != PLAYER_DIED_DURING_BOSS)) {
				// Boss was defeated!
				distns=stage_distns[level];		// do this BEFORE we increment, cause it's -1
                if (nDifficulty == DIFFICULTY_EASY) distns-=300;
                if (nDifficulty == DIFFICULTY_MEDIUM) distns-=150;
				level++; 
			}
		}
		if ((level==6)&&(joynum)) {
			SWITCH_IN_BANK8a;
			gamwin();
		}
		if (level==9) {
			if (joynum == 0) {
				score = oldscore;
			}
			goto titleagain;		// level 9 is a flag from gamovr()
		}
	}
}
 
void space()
{ /* do a space level */
	unsigned char idx;

	ispace();
	flag=MAIN_LOOP_ACTIVE;
	while ((flag == MAIN_LOOP_ACTIVE) || (flag == MAIN_LOOP_DONE)) { 
		// frame 0
		wrapplayer();   	// handles player and player shots
		
		SWITCH_IN_BANK2a;
		colchk(0);			// first half - this means the gnat may still beat mines..
		
		SWITCH_IN_BANK6a;
		stars();

		// frame 1
		SWITCH_IN_BANK14b;		// covers enout and enemy
		if (flag == MAIN_LOOP_ACTIVE) {
			enout();
		}
		enemy();

		SWITCH_IN_BANK6a;
		stars();

		// frame 2
		if (ch) {
    		SWITCH_IN_BANK2b;
			cheat();
		} else {
            SWITCH_IN_BANK2a;
			plycol();		// player collision first, otherwise the mines /never/ hit the gnat if it's shooting
		}
		colchk(1);			// second half
		
		SWITCH_IN_BANK6a;
		stars();
		
		if (flag == MAIN_LOOP_DONE) {
			// check if all enemies dead (ent[0-5])
			for (idx=0; idx<5; idx++) {
				if (ent[idx]!=(unsigned)ENEMY_NONE) break;
			}
			if ((idx >= 5) && (ptp4 == POWERUP_NONE)) break;
		} else {
			distns--;
			if (distns==0) {
				flag=MAIN_LOOP_DONE;
				pLoopMus = NULL;
				// fix up all mines into helicopters (typewise) so they leave the screen
				for (idx=0; idx<6; idx++) {
					if (ent[idx] == ENEMY_MINE) {
						ent[idx] = ENEMY_HELILEAVE;
						en_func[idx] = enemyhelileave;
					}
				}
				centr(11, "WARNING!! BOSS APPROACHING!");
			}
		}
	}
}

void ispace() {
	/* init space level (stars, music, etc) */
	/* actually it's any level now, but when first started this game was going to
	 * include mazes like Major Havoc. Don't think I will have that now...
	 */

	cls();
	spdall();
	screen(1);
	shutup();
	sgrint();	// sets color table
	memset(tmpbuf, 0, 8);
	patcpy(0, 32);	// clear the space characters
	screenFlashCnt = 0;

	// load the electric wall sprites and normal shots
    if (f18a) {
    	SWITCH_IN_BANK14a;
	    vdpmemcpy(gSPRITE_PATTERNS+76*8, F18ELECTRICWALL, 2*4*8);
	    vdpmemcpy(gSPRITE_PATTERNS+84*8, &F18SPRITES[84*8], 4*8);
	    vdpmemcpy(gSPRITE_PATTERNS+76*8+0x800, F18ELECTRICWALL2, 2*4*8);
	    vdpmemcpy(gSPRITE_PATTERNS+84*8+0x800, &F18SPRITES2[84*8], 4*8);
	    vdpmemcpy(gSPRITE_PATTERNS+76*8+0x1000, F18ELECTRICWALL3, 2*4*8);
	    vdpmemcpy(gSPRITE_PATTERNS+84*8+0x1000, &F18SPRITES3[84*8], 4*8);
        // turn on the extended sprite mode - 3 bitplanes
        VDP_SET_REGISTER(F18A_REG_ECM, 3);
    } else {
    	SWITCH_IN_BANK5b;
	    vdpmemcpy(gSPRITE_PATTERNS+76*8, ELECTRICWALL, 2*4*8);
	    vdpmemcpy(gSPRITE_PATTERNS+84*8, &SPRITES[84*8], 4*8);
    }

	// small stars first
	SWITCH_IN_BANK6a;
	initstars();

	// show score
	addscore(0);
	if ((playership != SHIP_SELENA) && (playership != SHIP_CRUISER)) {
        switch (playership) {
            case SHIP_LADYBUG:
        		hchar(23, 2, LIVESCHARLADYBUG, lives);
                break;
            case SHIP_GNAT:
        		hchar(23, 2, LIVESCHARGNAT, lives);
                break;
            default:
        		hchar(23, 2, LIVESCHARSSA, lives);
                break;
        }
	}

	// background color for cloaked enemies
    if (f18a) {
    	bgColor = PAL_INVISIBLE;
    } else {
    	bgColor = COLOR_BLACK;
    }

	// set up main music and any stage-specific stuff
	switch(level) {
	case 1:	StartMusic(STAGE1MUS, 1); break;
	case 2:	StartMusic(STAGE2MUS, 1); break;
	case 3:	StartMusic(STAGE3MUS, 1); break;
	case 4:	StartMusic(STAGE4MUS, 1); break;
	case 5:	StartMusic(STAGE5MUS, 1); break;
    case 6: StartMusic(WINANIMMUS, 0); break;
	}

	SWITCH_IN_BANK2b;
	playerinit();
	
	SWITCH_IN_BANK14b;
	enemyinit();

	distns=distns+100*level;
}
 
void gamovr()
{ /* do the GAME OVER thing */
	int nCnt;
	unsigned char btnok=0;

	centr(11,"GAME OVER");
	StartMusic(GAMEOVERMUS, 0);
	
	nCnt=300;
	level=7;			// flag for exit to restart
	spdall();

	for (;;) {
        if (!(isSNPlaying)) {
			shutup();
		} else {
			nCnt=240;
		}
	
        // using wrap will handle the bank switch for us
		wrapstars();

		nCnt--;
		if (nCnt == 0) {
			level=9;	// change flag to go back to title pic
			break;
		}

		kscanfast(joynum);
		if (KSCAN_KEY == JOY_FIRE) {
			if (btnok) {
				break;
			}
        } else if (KSCAN_KEY == '0') {
            if (btnok) {
                nCnt=1; // force early timeout
            }
        } else {
			btnok = 1;
		}
	}
}

// used by boss and player in multiple places to position and center player sprites
void playmv() { 
	/* locate the ship at SHIP_R,SHIPC (PLAYER_SPRITE already moved) */
	uint8 x;

	//sploct(PLAYER_SPRITE,r,c);
	sploct(PLAYER_SPRITE+1,SHIP_R,SHIP_C+16);
	sploct(PLAYER_SPRITE+2,SHIP_R+16,SHIP_C);
	sploct(PLAYER_SPRITE+3,SHIP_R+16,SHIP_C+16);

    if ((!f18a)||(shield > 0)) {
        sploct(PLAYER_SHIELD,SHIP_R,SHIP_C);
	    sploct(PLAYER_SHIELD+1,SHIP_R,SHIP_C+16);
	    sploct(PLAYER_SHIELD+2,SHIP_R+16,SHIP_C);
	    sploct(PLAYER_SHIELD+3,SHIP_R+16,SHIP_C+16);
    }
	
    sploct(PLAYER_FLAME,SHIP_R+flameOffset,SHIP_C+8);
	
	// set shield color
	if (shield != oldshield) {
        if (f18a) {
            unsigned int old = nBank;
            SWITCH_IN_BANK14a;

		    if (oldshield == 0) {
			    // new shield must not be zero, so turn on the shield patterns
			    shieldsOn();
		    }
            
		    if (shield > 70) {
                loadpal_f18a(F18SHIELDMAX, PAL_PLAYSHIELD*4+1, 7);
		    } else if (shield > 40) {   // also find the code that plays sfx_shieldwarn
                loadpal_f18a(F18SHIELDMED, PAL_PLAYSHIELD*4+1, 7);
		    } else if (shield > 0) {
                loadpal_f18a(F18SHIELDLOW, PAL_PLAYSHIELD*4+1, 7);
		    } else {
			    // done with shield, back to normal
			    shieldsOff();
		    }

            SWITCH_IN_PREV_BANK(old);
        } else {
		    if (oldshield == 0) {
			    // new shield must not be zero, so turn on the shield patterns
			    shieldsOn();
		    }
            
            if (shield > 70) {
			    x=COLOR_WHITE;
		    } else if (shield > 40) {   // also find the code that plays sfx_shieldwarn
			    x=COLOR_DKYELLOW;
		    } else if (shield > 0) {
			    x=COLOR_DKRED;
		    } else {
			    x=COLOR_BLACK;

			    // done with shield, back to normal
			    shieldsOff();
		    }

		    spcolr(PLAYER_SHIELD,x); 
		    spcolr(PLAYER_SHIELD+1,x); 
		    spcolr(PLAYER_SHIELD+2,x); 
		    spcolr(PLAYER_SHIELD+3,x);
        }

		oldshield = shield;
	}
}

// adds to score and displays the result
void addscore(unsigned int val) {
	unsigned int x;
	unsigned int c;

	score+=val;
	x=score;

	VDP_SET_ADDRESS_WRITE((unsigned int)(gIMAGE+13));

#if 0
	// This version does 1000 passes with values 0-255 in 179 ticks - 2983uS average
	// write the correct digits
	VDPWD=((x/10000)+'0');
	x%=10000;
	VDPWD=((x/1000)+'0');
	x%=1000;
	VDPWD=((x/100)+'0');
	x%=100;
	VDPWD=((x/10)+'0');
	// should be enough calculation delay here
	VDPWD=((x%10)+'0');
#else
	// this version does 1000 passes with 0-255 in 29 ticks - 483uS average (6x faster!)
	c='0';
	while (x>=10000) {
		++c;
		x-=10000;
	}
	VDPWD=c;

	c='0';
	while (x>=1000) {
		++c;
		x-=1000;
	}
	VDPWD=c;

	c='0';
	while (x>=100) {
		++c;
		x-=100;
	}
	VDPWD=c;

	c='0';
	while (x>=10) {
		++c;
		x-=10;
	}
	VDPWD=c;
	VDP_SAFE_DELAY();
	VDPWD=x+'0';
#endif

	VDP_SAFE_DELAY();
	VDPWD=('0');
//	VDP_SAFE_DELAY();
	VDPWD=('0'+scoremode);
}

// displays centered, NUL terminated text
void centr(unsigned int row, const char *out) {
	/* print&center the text in out[]*/
	unsigned int c;

	c=15-(strlen(out)>>1);
	VDP_SET_ADDRESS_WRITE(gIMAGE+(row<<5)+c);

	while (*out) {
		VDPWD=(*(out++));
	}
}

void sgrint()
{ /*init space colors */
	unsigned char a;

	for (a=0; a<32; a++) {
		color(a,15,0);
		color2(a,15,0);
	}

	// red color set for explosion, everything else is white (not in set 2)
	color(1,9,0);
}

// reboot wrapper that saves off some handy variables and restarts the game
void reboot() {
	unsigned char x;

	// to preserve the score while keeping the lazy reboot, we save it off above the stack
	// we save it twice, once inverted as a checksum, so the boot will always know it's right
	*((unsigned short*)tmpbuf) = score;
	*((unsigned short*)(&tmpbuf[2])) = ~score;
	memcpy(SAVEDSCORE, tmpbuf, 4);

	// also save off the scoremode (only 0-9, so we can store a count here too to init the seed with)
	*SAVEDMODE = (((*SAVEDMODE+0x10)&0xf0) | scoremode)&0xff;
	// save off the last attract ship too
	x = *SAVEDATTRACT;
	x = (attractShip<<4)|(x&1);
	*SAVEDATTRACT = x&0xff;

    // save off the detected F18A state (partly to save redetect, mostly to save if it was disabled)
    *SAVEDF18A = f18a&0xff;

	hwreboot();	// never returns
}

// replacement for memmove
void *memmove(void *dest, const void *src, unsigned int n) {
    unsigned char *f = (unsigned char *)src;
    unsigned char *t = (unsigned char *)dest;
    
    if ((f==t)||(n==0)) return dest;
    
    if ((t > f)&&(t-f < n)) {
        // overlap, so copy backwards - just doing bytes
        for (int i=n-1; i>=0; --i) {
            t[i]=f[i];
        }
        return dest;
    } else {
        // overlap or not, copy forwards which is slightly faster
        for (int i=0; i<n; ++i) {
            t[i]=f[i];
        }
        return dest;
    }
}

// libti99
#include <vdp.h>
#include <f18a.h>
#include <sound.h>
#include <kscan.h>
#include <TISNPlay.h>

// game
#include "game.h"
#include "trampoline.h"
#include "music.h"
#include "human.h"

// text for the ladybug lines
//                       12345678901234567890123456789012
const char LadyText[] = "What do you think bugs dream of?";
const char LadyText2[]= "Maybe flying somewhere exciting!";

unsigned int rndnum13(void);
void ladybugwin(void);

unsigned int rndnum13()
{
	static unsigned int seed = 1;
	if (seed == 0) ++seed;	// 0 is illegal

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

	// trying the dreamcast one again, but 13-bit this time (0-8191)
	if (seed&1) {
		seed >>= 1;
		seed ^= 0x1B00;
	} else {
		seed >>= 1;
	}

	return seed;
}

void ladybugwin() {
	unsigned char i2;

	i2=intpic();
	// change the init of the image table to all zeros (it's at >1800)
	vdpmemset(0x1800, 0, 768);
	// load the bug game screen (it won't show due to the altered SIT)
	wrapLoadLadyScreen();
    screen(COLOR_BLACK);    // make sure the screen is black

	// enable the screen
	VDP_SET_REGISTER(VDP_REG_MODE1, i2);
	FIX_KSCAN(i2);

	// start the music
	StartMusic(WINANIMMUS, 0);

	// we're going to scroll the screen into place, just as an analog for starting a new stage
	for (unsigned char i1=0; i1<24; ++i1) {
		// one frame
		VDP_WAIT_VBLANK_CRU;
		musicsync();	// clears it

		// update the SIT - there's no easy way to do a /correct/
		// scroll, but this somewhat deformed one actually looks
		// halfway decent! Completely wrong, but decent.
        // use the variable, not the macro
		vdpwriteinc(gImage, (23-i1)*32, (i1+1)*32);
		
		VDP_WAIT_VBLANK_CRU;
		musicsync();	// clears it
	}

	// wait just under two seconds
	for (unsigned char i1=0; i1<100; ++i1) {
		VDP_WAIT_VBLANK_CRU;
		musicsync();	// clears it
	}

	// to dither it in, we are going to need to use uncompressed image data

	// we'll fade in from the bottom left upwards
	// to maintain performance, we won't /fully/ run 8k over each step
	// but it should average out! Note the bottom row is blank in the data
	// so we can put the text there.
	// I really like this effect!!
	for (unsigned char step=0; step<32; ++step) {
		for (unsigned int i1=0; i1<512; ++i1) {
			unsigned int x = rndnum13();
			{
				unsigned char col = (x>>3)&0x1f;	// column is easy
				if (col < 31-step) continue;
			}
			{
				unsigned char row = ((x>>11)<<3)+((x&0x7ff)>>8);
				if ((step<23)&&(row < 23-step)) continue;
			}
			wrapLadyBugByte(x);
			musicsync();	// does not wait
		}
	}
	// the final full run to flesh it out
	for (unsigned int i1=0; i1<8191; ++i1) {
		unsigned int x = rndnum13();
		wrapLadyBugByte(x);
		musicsync();	// does not wait
	}
	wrapLadyBugByte(0);

	// wait a few seconds
	for (unsigned char i1=0; i1<150; ++i1) {
		VDP_WAIT_VBLANK_CRU;
		musicsync();	// clears it
	}

	// draw the text on the bottom row
	{
		const char *pTxt = LadyText;
		int off = 0x1700;
		while (*pTxt) {
			wrapDrawLastRowText(*pTxt, off);
			++pTxt;
			off+=8;
			VDP_WAIT_VBLANK_CRU;
			musicsync();	// clears it
			VDP_WAIT_VBLANK_CRU;
			musicsync();	// clears it
		}
	}

    // wait a few cycles
	for (unsigned int i1=0; i1<330; ++i1) {
		VDP_WAIT_VBLANK_CRU;
		musicsync();	// clears it
	}

    // wipe the text line
    vdpmemset(0x1700, 0, 32*8);
	VDP_WAIT_VBLANK_CRU;
	musicsync();	// clears it

    // wait a few cycles
	for (unsigned char i1=0; i1<60; ++i1) {
		VDP_WAIT_VBLANK_CRU;
		musicsync();	// clears it
	}

    // draw more text on the bottom row
	{
		const char *pTxt = LadyText2;
		int off = 0x1700;
		while (*pTxt) {
			wrapDrawLastRowText(*pTxt, off);
			++pTxt;
			off+=8;
			VDP_WAIT_VBLANK_CRU;
			musicsync();	// clears it
			VDP_WAIT_VBLANK_CRU;
			musicsync();	// clears it
		}
	}
}



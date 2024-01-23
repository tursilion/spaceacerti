#include <vdp.h>
#include <sound.h>
#include <kscan.h>

#include "game.h"
#include "trampoline.h"
#include "music.h"

#define BIN2INC_HEADER_ONLY
#include "title_c.c"
#include "title_p.c"
#include "../title2/ship1_c.c"
#include "../title2/ship1_p.c"
#include "../title2/ship2_c.c"
#include "../title2/ship2_p.c"
#include "../title2/ship3_c.c"
#include "../title2/ship3_p.c"
#include "../title2/ship4_c.c"
#include "../title2/ship4_p.c"

// bitmap for F18A text
const unsigned char F18Text[] = {
    0x7E,0xC0,0xC0,0xDC,0xC0,0xC0,0xC0,0x00,
    0x18,0x38,0x18,0x18,0x18,0x18,0x18,0x00,
    0x7C,0xC6,0xC6,0x5C,0xC6,0xC6,0x7C,0x00,
    0x7C,0xC6,0xC6,0xDE,0xC6,0xC6,0xC6,0x00
};

extern checkQuit();

// only ship1 is on the same page as we are, 2-4 are on another page

// this is for bitmap mode - it does NOT use the globals,
// but uses fixed offsets
void ldpic() {
	// loads a picture - we aren't actually loading from disk
	// we are un-RLE-ing the image from ROM
	RLEUnpack(0x0000, TITLEP, 6144);
	RLEUnpack(0x2000, TITLEC, 6144);
}

void animate(unsigned char x) {
	unsigned char i;

	switch (x) {
		case 0:
			wrapCopyShip(SHIP1P, SHIP1C, 1);
			break;

		case 1:
			wrapCopyShip(SHIP2P,SHIP2C,2);
			break;

		case 2:
			wrapCopyShip(SHIP3P,SHIP3C,3);
			break;

		case 3:
			wrapCopyShip(SHIP4P,SHIP4C,4);
			break;
	}
}

void handleTitlePage() {
	unsigned char i;
	unsigned int cntdown;

	shutup();

	i=intpic();
	ldpic();
	spdall();
	// enable the screen
	VDP_SET_REGISTER(VDP_REG_MODE1, i);
	FIX_KSCAN(i);
	
	// display score in sprites, if not zero
	// assumes sprite patterns at 0x3800
	if ((playership != 255)&&(score != 0)) {
		wrapspritescore(0x3800, 0x1b00, 0xff, 103, 0);
	}

    // display F18A in bottom left if active
    if (f18a) {
        vdpmemcpy(0x1000+7*32*8, F18Text, 4*8);
        vdpmemset(0x3000+7*32*8, COLOR_LTBLUE<<4|COLOR_BLACK, 32);
    }

	cntdown = 600;
	joynum=1;
	do {
		seed++;			// random number seed

		if (--cntdown == 0) {
			joynum=0;	// demo mode
			break;
		}

		// every 4 frames, update the graphic frame
		vdpwaitvint();
		if (0 == (seed&0x03)) {
			animate((seed>>2)&0x03);
		}

        checkQuit();
		joynum=joynum==1?2:1;
		kscanfast(joynum);

        if (KSCAN_KEY == '0') cntdown=1;
	} while (JOY_FIRE != KSCAN_KEY);
	
	// wipe the screen
	for (i=0; i<12; i++) {
		vdpmemset(0x1800+384+((int)i << 5), 0x00, 32);
		vdpmemset(0x1800+384-((int)i << 5), 0x00, 32);

		// wait for vblank
		vdpwaitvint();
	}
}


// libti99 
#include <vdp.h>
#include <sound.h>
#include <kscan.h>
#include <f18a.h>

// game
#include "game.h"
#include "trampoline.h"
#include "enemy.h"
#include "music.h"
#include "human.h"
#include "boss.h"
#include "f18load.h"

#define BIN2INC_HEADER_ONLY
#include "f18abosses.c"

extern unsigned int bossShape[8*2];

// some graphics copies so we don't need to page flip for graphics during the final boss
// well, so much for that ;)
const unsigned char beamleftgfx[] = {
	// beam moving left (104)
	0x00,0x00,0x00,0x00,0x04,0x08,0x19,0x11,0x11,0x19,0x08,0x04,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x20,0x40,0x80,0x80,0x00,0x00,0x80,0x80,0x40,0x20,0x00,0x00,0x00
};
const unsigned char f18beamleftgfx[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
const unsigned char f18beamleftgfx2[] = {
    0x00,0x00,0x00,0x00,0x04,0x08,0x19,0x11,0x11,0x19,0x08,0x04,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x20,0x40,0x80,0x80,0x00,0x00,0x80,0x80,0x40,0x20,0x00,0x00,0x00,
};

const unsigned char hominggfx[] = {
    0x00,0x00,0x00,0x03,0x04,0x0B,0x17,0x17,0x17,0x17,0x0B,0x04,0x03,0x00,0x00,0x00,
    0x00,0x00,0x00,0xC0,0x20,0xD0,0xE8,0xE8,0xE8,0xE8,0xD0,0x20,0xC0,0x00,0x00,0x00
};
const unsigned char f18hominggfx[] = {
    0x00,0x00,0x00,0x03,0x04,0x0B,0x17,0x17,0x17,0x17,0x0B,0x04,0x03,0x00,0x00,0x00,
    0x00,0x00,0x00,0xC0,0x20,0xD0,0xE8,0xE8,0xE8,0xE8,0xD0,0x20,0xC0,0x00,0x00,0x00
};
const unsigned char f18hominggfx2[] = {
    0x00,0x00,0x00,0x00,0x03,0x07,0x0F,0x0F,0x0F,0x0F,0x07,0x03,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0xC0,0xE0,0xF0,0xF0,0xF0,0xF0,0xE0,0xC0,0x00,0x00,0x00,0x00
};
const unsigned char f18hominggfx3[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

// boss shapes for cockpit (per row, min/max character)
const unsigned int bossShape1[8*2] = {
    2*8,7*8-8,
    0*8,9*8-8,
    0*8,9*8-8,
    0*8,9*8-8,
    0*8,9*8-8,
    2*8,7*8-8,
    3*8,6*8-8,
    4*8,5*8-8
};
const unsigned int bossShape2[11*2] = {
    0*8,10*8-8,
    0*8,10*8-8,
    2*8,7*8-8,
    3*8,6*8-8,
    4*8,5*8-8,
    4*8,5*8-8,
    3*8,6*8-8,
    3*8,6*8-8,
    3*8,6*8-8,
    3*8,6*8-8,
    4*8,5*8-8
};
const unsigned int bossShape3[7*2] = {
    0*8,9*8-8,
    0*8,9*8-8,
    0*8,9*8-8,
    0*8,9*8-8,
    0*8,9*8-8,
    1*8,8*8-8,
    2*8,7*8-8
};
const unsigned int bossShape4[12*2] = {
    0*8,10*8-8,
    0*8,10*8-8,
    0*8,10*8-8,
    0*8,10*8-8,
    0*8,10*8-8,
    0*8,10*8-8,
    3*8,5*8-8,
    3*8,6*8-8,
    2*8,7*8-8,
    2*8,7*8-8,
    2*8,7*8-8,
    3*8,6*8-8
};
const unsigned int bossShape5[9*2] = {
    0*8,13*8-8,
    0*8,13*8-8,
    0*8,13*8-8,
    0*8,13*8-8,
    1*8,12*8-8,
    2*8,11*8-8,
    4*8,9*8-8,
    5*8,8*8-8,
    6*8,7*8-8
};

void loadBossGfx() {
    if (f18a) {
    	vdpmemcpy(104*8+0x0800, f18hominggfx, 4*8);	// load sprite patterns
    	vdpmemcpy(104*8+0x1000, f18hominggfx2, 4*8);	// load sprite patterns
    	vdpmemcpy(104*8+0x1800, f18hominggfx3, 4*8);	// load sprite patterns
    } else {
    	vdpmemcpy(104*8+0x0800, hominggfx, 4*8);	// load sprite patterns
    }

    // copy the appropriate boss graphics
    // F18A will ignore the bossdraw function
	switch (level) {
		default:
		case 1:	memcpy(bossShape, bossShape1, sizeof(bossShape)); break;
		case 2:	memcpy(bossShape, bossShape2, sizeof(bossShape)); break;
		case 3:	memcpy(bossShape, bossShape3, sizeof(bossShape)); break;
		case 4:	memcpy(bossShape, bossShape4, sizeof(bossShape)); break;
		case 5:	memcpy(bossShape, bossShape5, sizeof(bossShape)); break;
	}
}

void restoreBossGfx() {
    if (f18a) {
    	vdpmemcpy(104*8+0x0800, f18beamleftgfx, 4*8);	// load sprite patterns
    	vdpmemcpy(104*8+0x1000, f18beamleftgfx2, 4*8);	// load sprite patterns
    } else {
    	vdpmemcpy(104*8+0x0800, beamleftgfx, 4*8);	// load sprite patterns
    }
}

void warpout() {
    int qw, tmp;
    int a, x;

	for (qw=0; qw<81; qw++){ 
		x=11186/(qw+200);
		a=x&0xf;	// low nibble of freq

		tmp=(x>>4)+(a<<8)+0xc000;
		SOUND=0xf0;
		SOUND=(tmp>>8);
		SOUND=tmp&0xff;
		SOUND=0xe7;
		SOUND=0xdf;

		wrapPlayerFlameBig();
		if ((SHIP_R<192)||(SHIP_R>200)) { 
			SHIP_R-=6; 
            playmv(); 
		} else {
			spdall();
		}
		wrapstars();
		SOUND=0xff;
	}
}
 
// erase a character on all tables for the boss
void AddDestroyed(unsigned int ptr) {
 	// clear out 8 bytes at ptr (like AddDamage, but no noise or random)
 	// also need to shift it through the other 3 tables
 	unsigned char idx,idx2;
	unsigned short mask = 0x00ff;

	// do initial pattern
	memset(tmpbuf, 0, 8);
	vdpmemset(ptr, 0, 8);

	// do the rest of the patterns
	for (idx=0; idx<3; idx++) {
 		ptr+=SCROLL_OFFSET;				// offset is to the next table
 		vdpmemread(ptr, tmpbuf, 16);
		mask>>=2;		// shift 2 pixels right
		mask|=0xc000;	// preserve shifted in pixels
 		for (idx2=0; idx2<8; idx2++) {
 			tmpbuf[idx2]&=(mask>>8)&0xff;
 			tmpbuf[idx2+8]&=mask&0xff;
 		}
 		vdpmemcpy(ptr,tmpbuf,16);
 	}
}

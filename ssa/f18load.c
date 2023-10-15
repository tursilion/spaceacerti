#include <vdp.h>
#include <f18a.h>
#include "f18load.h"
#define BIN2INC_HEADER_ONLY
#include "f18sprites.c"

// Load and execute at >3F00 in VDP
const unsigned char gpucode[] = {
    // loader - loads at 0x3f00
    0x02,0x00,0x3f,0x16,0x02,0x01,0x40,0x00,0x02,0x02,0x00,0x80,0xcc,0x70,0x06,0x02,
    0x16,0xfd,0x04,0x60,0x40,0x00,

    // main loop - runs at 0x4000
    0x05,0xa0,0x3c,0x86,0xc0,0x20,0x3c,0x84,0x02,0x40,0x00,0xff,0x13,0xf9,0xc8,0x20,
    0x3c,0x80,0x3c,0x8a,0xc8,0x20,0x3c,0x84,0x3c,0x88,0x04,0xe0,0x3c,0x84,0xd0,0x60,
    0x3c,0x86,0x16,0x04,0x02,0x01,0x01,0x00,0xc8,0x01,0x3c,0x86,0xc0,0x60,0x3c,0x88,
    0x02,0x41,0x00,0xff,0x0a,0x11,0xc0,0xa1,0x3f,0x52,0x04,0x52,

    // jump table
    0x00,0x00,0x40,0x40,0x40,0xae,
    
    // boss damage add
    0x02,0x02,0x00,0x08,0x02,0x00,0x3c,0x90,0xc0,0x60,0x3c,0x86,0x02,0x03,
    0x01,0x00,0x20,0x43,0x16,0x05,0x09,0x11,0x02,0x03,0xb8,0x00,0x28,0x43,0x10,0x01,
    0x09,0x11,0xc8,0x01,0x3c,0x86,0x02,0x61,0x00,0xff,0x05,0x41,0xcc,0x01,0x06,0x02,
    0x16,0xec,0x02,0x00,0xff,0x00,0xc8,0x00,0x3c,0x98,0xc0,0x20,0x3c,0x8a,0x02,0x03,
    0x00,0x04,0x02,0x01,0x3c,0x90,0x02,0x02,0x00,0x08,0xc1,0x11,0x54,0x04,0x06,0xc4,
    0x02,0x20,0x00,0x08,0x54,0x04,0x02,0x20,0xff,0xf9,0x06,0xc4,0x09,0x24,0xcc,0x44,
    0x06,0x02,0x16,0xf3,0x02,0x20,0x07,0xf8,0x06,0x03,0x16,0xeb,0x10,0xa9,

    // memset
    0xc0,0x20,0x3c,0x8a,0xc0,0x60,0x3c,0x88,0xc0,0xa0,0x3c,0x82,0xdc,0x01,0x06,0x02,
    0x16,0xfd,0x10,0x9f

};

#define GPULOADADR 0x3f00
void initF18GPU() {
    VDP_SET_REGISTER(F18A_REG_MAXSPR, 31);  // go ahead and disable flicker, we don't need it
    VDP_SET_REGISTER(F18A_REG_EXTRAPAL, 0); // default modes use the default palette
    VDP_SET_REGISTER(F18A_REG_SIZES, 0);    // 2k table sizes for pattern tables
    vdpmemcpy(GPULOADADR, gpucode, sizeof(gpucode));    // and load the GPU
    // clear out the input registers
    VDP_SET_ADDRESS_WRITE(GPU_DAMAGEGO);
    VDPWD = 0;  // no need for delays on F18A
    VDPWD = 0;
    startgpu_f18a(GPULOADADR);
}

void initCruiserf18() {
    // add 32 to VDP address, 16 to sprite table address
    vdpmemcpy(108*8+0x0800, &F18SPRITES[108*8], 4*4*8);	    // ship straight sprites layer 1
    vdpmemcpy(140*8+0x0800, &F18SPRITES[124*8], 4*4*8);	    // ship left sprites layer 1
    vdpmemcpy(172*8+0x0800, &F18SPRITES[140*8], 4*4*8);	    // ship right sprites layer 1
    vdpmemcpy(108*8+0x1000, &F18SPRITES2[108*8], 4*4*8);	// ship straight sprites layer 2
    vdpmemcpy(140*8+0x1000, &F18SPRITES2[124*8], 4*4*8);	// ship left sprites layer 2
    vdpmemcpy(172*8+0x1000, &F18SPRITES2[140*8], 4*4*8);	// ship right sprites layer 2
    // shields
    vdpmemcpy(124*8+0x0800, F18ALTSHIELDS, 4*4*8);			// straight
	vdpmemcpy(156*8+0x0800, &F18ALTSHIELDS[4*4*8], 4*4*8);	// left
	vdpmemcpy(188*8+0x0800, &F18ALTSHIELDS[8*4*8], 4*4*8);	// right
    vdpmemcpy(124*8+0x1000, F18ALTSHIELDS2, 4*4*8);			// straight
	vdpmemcpy(156*8+0x1000, &F18ALTSHIELDS2[4*4*8], 4*4*8);	// left
	vdpmemcpy(188*8+0x1000, &F18ALTSHIELDS2[8*4*8], 4*4*8);	// right
}

void initSnowballf18() {
    // add 32 to VDP address, 16 to sprite table address
    vdpmemcpy(108*8+0x0800, &F18SNOWBALL[0*8], 4*4*8);	    // ship straight sprites layer 1
    vdpmemcpy(140*8+0x0800, &F18SNOWBALL[16*8], 4*4*8);	    // ship left sprites layer 1
    vdpmemcpy(172*8+0x0800, &F18SNOWBALL[32*8], 4*4*8);	    // ship right sprites layer 1
    vdpmemcpy(108*8+0x1000, &F18SNOWBALL2[0*8], 4*4*8);	    // ship straight sprites layer 2
    vdpmemcpy(140*8+0x1000, &F18SNOWBALL2[16*8], 4*4*8);	// ship left sprites layer 2
    vdpmemcpy(172*8+0x1000, &F18SNOWBALL2[32*8], 4*4*8);	// ship right sprites layer 2
    // shields
	vdpmemcpy(124*8+0x0800, F18ALTSNOWBALL, 4*4*8);			// straight
	vdpmemcpy(156*8+0x0800, &F18ALTSNOWBALL[4*4*8], 4*4*8);	// left
	vdpmemcpy(188*8+0x0800, &F18ALTSNOWBALL[8*4*8], 4*4*8);	// right
	vdpmemcpy(124*8+0x1000, F18ALTSNOWBALL2, 4*4*8);		// straight
	vdpmemcpy(156*8+0x1000, &F18ALTSNOWBALL2[4*4*8], 4*4*8);// left
	vdpmemcpy(188*8+0x1000, &F18ALTSNOWBALL2[8*4*8], 4*4*8);// right
}

void initLadybugf18() {
    // add 32 to VDP address, 16 to sprite table address
    vdpmemcpy(108*8+0x0800, &F18LADYBUG[0*8], 4*4*8);	    // ship straight sprites layer 1
    vdpmemcpy(140*8+0x0800, &F18LADYBUG[16*8], 4*4*8);	    // ship left sprites layer 1
    vdpmemcpy(172*8+0x0800, &F18LADYBUG[32*8], 4*4*8);	    // ship right sprites layer 1
    vdpmemcpy(108*8+0x1000, &F18LADYBUG2[0*8], 4*4*8);	    // ship straight sprites layer 2
    vdpmemcpy(140*8+0x1000, &F18LADYBUG2[16*8], 4*4*8);	    // ship left sprites layer 2
    vdpmemcpy(172*8+0x1000, &F18LADYBUG2[32*8], 4*4*8);	    // ship right sprites layer 2

    vdpmemcpy(124*8+0x0800, F18ALTLADYBUG, 4*4*8);			// straight
	vdpmemcpy(156*8+0x0800, &F18ALTLADYBUG[4*4*8], 4*4*8);	// left
	vdpmemcpy(188*8+0x0800, &F18ALTLADYBUG[8*4*8], 4*4*8);	// right
	vdpmemcpy(124*8+0x1000, F18ALTLADYBUG2, 4*4*8);			// straight
	vdpmemcpy(156*8+0x1000, &F18ALTLADYBUG2[4*4*8], 4*4*8);	// left
	vdpmemcpy(188*8+0x1000, &F18ALTLADYBUG2[8*4*8], 4*4*8);	// right
}

void initGnatf18() {
    // add 32 to VDP address, 16 to sprite table address
    vdpmemcpy(108*8+0x0800, &F18GNAT[0*8], 4*4*8);	    // ship straight sprites layer 1
    vdpmemcpy(140*8+0x0800, &F18GNAT[16*8], 4*4*8);	    // ship left sprites layer 1
    vdpmemcpy(172*8+0x0800, &F18GNAT[32*8], 4*4*8);	    // ship right sprites layer 1
    vdpmemcpy(108*8+0x1000, &F18GNAT2[0*8], 4*4*8);	    // ship straight sprites layer 2
    vdpmemcpy(140*8+0x1000, &F18GNAT2[16*8], 4*4*8);	// ship left sprites layer 2
    vdpmemcpy(172*8+0x1000, &F18GNAT2[32*8], 4*4*8);	// ship right sprites layer 2

    vdpmemset(100*8+0x0800, 0, 4*8);					// zero the flame sprites by default
    vdpmemset(100*8+0x1000, 0, 4*8);					// zero the flame sprites by default

    vdpmemcpy(124*8+0x0800, F18ALTGNAT, 4*4*8);			    // straight
	vdpmemcpy(156*8+0x0800, &F18ALTGNAT[4*4*8], 4*4*8);	    // left
	vdpmemcpy(188*8+0x0800, &F18ALTGNAT[8*4*8], 4*4*8);	    // right
    vdpmemcpy(124*8+0x1000, F18ALTGNAT2, 4*4*8);			// straight
	vdpmemcpy(156*8+0x1000, &F18ALTGNAT2[4*4*8], 4*4*8);	// left
	vdpmemcpy(188*8+0x1000, &F18ALTGNAT2[8*4*8], 4*4*8);	// right
}

void initSelenaf18() {
    // add 32 to VDP address, 16 to sprite table address
    vdpmemcpy(108*8+0x0800, &F18SELENA[0*8], 4*4*8);	    // ship straight sprites layer 1
    vdpmemcpy(140*8+0x0800, &F18SELENA[16*8], 4*4*8);	    // ship left sprites layer 1
    vdpmemcpy(172*8+0x0800, &F18SELENA[32*8], 4*4*8);	    // ship right sprites layer 1
    vdpmemcpy(108*8+0x1000, &F18SELENA2[0*8], 4*4*8);	    // ship straight sprites layer 2
    vdpmemcpy(140*8+0x1000, &F18SELENA2[16*8], 4*4*8);	    // ship left sprites layer 2
    vdpmemcpy(172*8+0x1000, &F18SELENA2[32*8], 4*4*8);	    // ship right sprites layer 2
    vdpmemcpy(96*8+0x0800, F18HOMING, 4*8);             // homing shot
    vdpmemcpy(96*8+0x1000, F18HOMING2, 4*8);            // homing shot
    vdpmemset(100*8+0x0800, 0, 4*8);					// zero the flame sprites
    vdpmemset(100*8+0x1000, 0, 4*8);					// zero the flame sprites

    vdpmemcpy(124*8+0x0800, F18ALTSELENA, 4*4*8);			// straight
	vdpmemcpy(156*8+0x0800, &F18ALTSELENA[4*4*8], 4*4*8);	// left
	vdpmemcpy(188*8+0x0800, &F18ALTSELENA[8*4*8], 4*4*8);	// right
    vdpmemcpy(124*8+0x1000, F18ALTSELENA2, 4*4*8);			// straight
	vdpmemcpy(156*8+0x1000, &F18ALTSELENA2[4*4*8], 4*4*8);	// left
	vdpmemcpy(188*8+0x1000, &F18ALTSELENA2[8*4*8], 4*4*8);	// right
}

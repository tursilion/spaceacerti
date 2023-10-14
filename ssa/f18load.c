#include <vdp.h>
#include "f18load.h"
#define BIN2INC_HEADER_ONLY
#include "f18sprites.c"

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

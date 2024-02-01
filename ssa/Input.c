// joystfast adapted from libti99all and my own port of mario bros
// This is so I can transparently support keyboard or joystick input, and speed up the scan a bit
// to just what I care about. kscanfast already has both in it, so that's fine

#include <kscan.h>

void joystfast(unsigned char unit) {
	unsigned int result;
    unsigned int key;

	// read the joystick lines (column 6 or 7, (5 added to unit))
	__asm__ volatile ("li r12,>0024\n\tai %1,>0500\n\tldcr %1,3\n\tsrc r12,7\n\tli r12,>0006\n\tclr %0\n\tstcr %0,8" : "=r"(result) : "r"(unit) : "r12");

	KSCAN_JOYY = 0;
	KSCAN_JOYX = 0;

	if ((result & 0x0200) == 0) KSCAN_JOYX = JOY_LEFT;
	if ((result & 0x0400) == 0) KSCAN_JOYX = JOY_RIGHT;
	if ((result & 0x0800) == 0) KSCAN_JOYY = JOY_DOWN;
	if ((result & 0x1000) == 0) KSCAN_JOYY = JOY_UP;

    if ((KSCAN_JOYX|KSCAN_JOYY)==0) {
        // read ESDX then
        // CRU rows: 20 18 16 14 , 12 10 8 6
        // S        Col 1, mask 0x2000 
        // X        Col 1, mask 0x8000
        // D        Col 2, mask 0x2000
        // E        Col 2, mask 0x4000
        // hard coded col and read
        // remember less than 9 bits, it's a byte address and needs to be in the MSB
            
        // check S and X (3 bits - X=0x4, S=0x1)
        __asm__("li r0,>0100\n\tli r12,>0024\n\tldcr r0,3\n\tsrc r12,7\n\tli r12,16\n\tclr %0\n\tstcr %0,3" : "=r"(key) : : "r12","cc");
        if ((key&0x0400)==0) KSCAN_JOYY = JOY_DOWN;
        if ((key&0x0100)==0) KSCAN_JOYX = JOY_LEFT;

        // check E and D (2 bits - E=0x2, D=0x1)
        __asm__("li r0,>0200\n\tli r12,>0024\n\tldcr r0,3\n\tsrc r12,7\n\tli r12,16\n\tclr %0\n\tstcr %0,2" : "=r"(key) : : "r12","cc");
        if ((key&0x0200)==0) KSCAN_JOYY = JOY_UP;
        if ((key&0x0100)==0) KSCAN_JOYX = JOY_RIGHT;
    }
}

// By columns, then rows. 8 Rows per column. No shift states
const unsigned char keymap[] = {
		61,32,13,255,1,2,3,255,
		'.','L','O','9','2','S','W','X',
		',','K','I','8','3','D','E','C',
		'M','J','U','7','4','F','R','V',
		'N','H','Y','6','5','G','T','B',
		'/',';','P','0','1','A','Q','Z'
};

void kscanfast(unsigned char mode) {
	KSCAN_KEY = 0xff;
	
    if (mode > 0) {
		unsigned int key;

		int col = 0x0600;		// joystick 1 fire column

		if (mode == 2) {
			col = 0x0700;		// make that joystick 2
		}

		__asm__ volatile ("li r12,>0024\n\tldcr %1,3\n\tsrc r12,7\n\tli r12,>0006\n\tclr %0\n\tstcr %0,1" : "=r"(key) : "r"(col) : "r12","cc");	// set cru, column, delay, read (only need 1 bit)
		if (key == 0) {
			KSCAN_KEY = 18;
		} else {
            // remember less than 9 bits, it's a byte address and needs to be in the MSB
            // if not the joystick, then check for Q/Y (everyone expects this)
            unsigned int mask = 0x4000;
            col = 0x0500;       // Q
            if (mode == 2) {
                col = 0x0400;   // Y
                mask = 0x0400;
            }
            
            __asm__ volatile ("li r12,>0024\n\tldcr %1,3\n\tsrc r12,7\n\tli r12,>0006\n\tclr %0\n\tstcr %0,7" : "=r"(key) : "r"(col) : "r12","cc");	// set cru, column, delay, read (need 7 bits)
            if ((key&mask) == 0) {
                KSCAN_KEY = 18;
            }

            // one last - also check for period (col 1, mask 0100) (col and mask are hard coded here)
            // CRU rows: 20 18 16 14 , 12 10 8 6
            __asm__("li r0,>0100\n\tli r12,>0024\n\tldcr r0,3\n\tsrc r12,7\n\tli r12,6\n\tclr %0\n\tstcr %0,1" : "=r"(key) : : "r12","cc");
            if (key==0) KSCAN_KEY=18;
        }
		if (KSCAN_KEY == 18) {
	        return;
		}
		// else fall through and read the keyboard
	}

    // otherwise read the keyboard
	{
		for (unsigned int col=0; col < 0x0600; col += 0x0100) {
			unsigned int key;
			__asm__ ("li r12,>0024\n\tldcr %1,3\n\tsrc r12,7\n\tli r12,>0006\n\tclr %0\n\tstcr %0,8" : "=r"(key) : "r"(col) : "r12","cc");	// set cru, column, delay, read
			unsigned int shift=0x8000;

			for (int cnt=7; cnt>=0; cnt--) {
				// a pressed key returns a 0 bit
				if (key & shift) {
					shift>>=1;
					continue;
				}
				// found one
				KSCAN_KEY = keymap[(col>>5)+cnt];
				return;
			}
		}
	}
}


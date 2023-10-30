// Just the boss draw functions, as they are very slow to compile

// libti99 
#include <vdp.h>

// game
#include "game.h"
#include "boss.h"

// boss.c
extern char br,bd;			// these ones need to be signed
extern unsigned char bc;	// but this one doesn't
extern unsigned int BNR,BNC;

// hard-coded boss draw functions are nearly twice as
// fast as the looping version, and we optimize a bit
// by skipping empty space.
// Note that the draw functions all have one more character
// than the count, because the shifting makes all chars 2 wide
// the compiler is optimizing these as "ld a,#<constant> ; out (_VDPWD),a"
// To that end, maybe we can save a few nops? ld a,# is 7 cycles, each
// nop is 4. out is 11. We need 29. So we have 18 cycles in instruction,
// and need 3 nops to get to 30. Safe delay is 5, so let's do our own
// and get the boss draw just a bit faster.
// None of these are used by the F18A

inline void BOSS_SAFE_DELAY(void) {	} 

#define EDGEBLANKA			\
	if (bd>0) {				\
		VDPWD=32;			\
		BOSS_SAFE_DELAY();	\
		VDPWD=32;			\
		BOSS_SAFE_DELAY();	\
	}

#define EDGEBLANKB			\
	if (bd<0) {				\
		VDPWD=32;			\
		BOSS_SAFE_DELAY();	\
		VDPWD=32;			\
	}

#define LINECHAR		\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();

#define LINECHAR2		\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();

#define LINECHAR3		\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();

#define LINECHAR4		\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();

#define LINECHAR5		\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();

#define LINECHAR6		\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();

#define LINECHAR7		\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();

#define LINECHAR8		\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();

#define LINECHAR9		\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();

#define LINECHAR10		\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();

#define LINECHAR12		\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();

#define LINECHAR14		\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();	\
	VDPWD=ch++;			\
	BOSS_SAFE_DELAY();

inline void BOSS_SET_ADDRESS_WRITE(unsigned int x)	{	VDPWA=((x)&0xff); VDPWA=((((x)>>8)&0x3f)|0x40); }

void draw4() {
	unsigned char ch=BOSS_START;
	unsigned int p;

	p=(((unsigned)(bc))>>2)+(((unsigned)(br))<<5)+gIMAGE;
	if (bd>0) {
		p-=2;	// for leading edge
	}

	BOSS_SET_ADDRESS_WRITE(p);
	EDGEBLANKA;
	LINECHAR10;
	EDGEBLANKB;
	//ch+=1-1;
	p+=32;

	BOSS_SET_ADDRESS_WRITE(p);
	EDGEBLANKA;
	LINECHAR10;
	EDGEBLANKB;
	//ch+=1-1;
	p+=32;

	BOSS_SET_ADDRESS_WRITE(p);
	EDGEBLANKA;
	LINECHAR10;
	EDGEBLANKB;
	//ch+=1-1;
	p+=32;

	BOSS_SET_ADDRESS_WRITE(p);
	EDGEBLANKA;
	LINECHAR10;
	EDGEBLANKB;
	//ch+=1-1;
	p+=32;

	BOSS_SET_ADDRESS_WRITE(p);
	EDGEBLANKA;
	LINECHAR10;
	EDGEBLANKB;
	//ch+=1-1;
	p+=32;

	BOSS_SET_ADDRESS_WRITE(p);
	EDGEBLANKA;
	LINECHAR10;
	EDGEBLANKB;
	ch+=1+3-1;
	p+=32;

	BOSS_SET_ADDRESS_WRITE(p+3);
	EDGEBLANKA;
	LINECHAR4;
	EDGEBLANKB;
	ch+=4+2-1;
	p+=32;

	BOSS_SET_ADDRESS_WRITE(p+2);
	EDGEBLANKA;
	LINECHAR5;
	EDGEBLANKB;
	ch+=4+2-1;
	p+=32;

	BOSS_SET_ADDRESS_WRITE(p+2);
	EDGEBLANKA;
	LINECHAR6;
	EDGEBLANKB;
	ch+=3+2-1;
	p+=32;

	BOSS_SET_ADDRESS_WRITE(p+2);
	EDGEBLANKA;
	LINECHAR6;
	EDGEBLANKB;
	ch+=3+2-1;
	p+=32;

	BOSS_SET_ADDRESS_WRITE(p+2);
	EDGEBLANKA;
	LINECHAR6;
	EDGEBLANKB;
	ch+=3+2-1;
	p+=32;

	BOSS_SET_ADDRESS_WRITE(p+2);
	EDGEBLANKA;
	LINECHAR5;
	EDGEBLANKB;
}

void draw5() {
	unsigned char ch=BOSS_START;
	unsigned int p;

	p=(((unsigned)(bc))>>2)+(((unsigned)(br))<<5)+gIMAGE;
	if (bd>0) {
		p-=2;	// for leading edge
	}

	BOSS_SET_ADDRESS_WRITE(p);
	EDGEBLANKA;
	LINECHAR14;
	EDGEBLANKB;
	//ch+=1+0-1;
	p+=32;

	BOSS_SET_ADDRESS_WRITE(p);
	EDGEBLANKA;
	LINECHAR14;
	EDGEBLANKB;
	//ch+=1+0-1;
	p+=32;

	BOSS_SET_ADDRESS_WRITE(p);
	EDGEBLANKA;
	LINECHAR14;
	EDGEBLANKB;
	//ch+=1+0-1;
	p+=32;

	BOSS_SET_ADDRESS_WRITE(p);
	EDGEBLANKA;
	LINECHAR14;
	EDGEBLANKB;
	//ch+=1+0-1;
	p+=32;

	BOSS_SET_ADDRESS_WRITE(p);
	EDGEBLANKA;
	LINECHAR14;
	EDGEBLANKB;
	ch+=1+1-1;
	p+=32;

	BOSS_SET_ADDRESS_WRITE(p+1);
	EDGEBLANKA;
	LINECHAR12;
	EDGEBLANKB;
	ch+=2+3-1;
	p+=32;

	BOSS_SET_ADDRESS_WRITE(p+3);
	EDGEBLANKA;
	LINECHAR8;
	EDGEBLANKB;
	ch+=4+4-1;
	p+=32;

	BOSS_SET_ADDRESS_WRITE(p+4);
	EDGEBLANKA;
	LINECHAR6;
	EDGEBLANKB;
	ch+=5+5-1;
	p+=32;

	BOSS_SET_ADDRESS_WRITE(p+5);
	EDGEBLANKA;
	LINECHAR4;
	EDGEBLANKB;
}

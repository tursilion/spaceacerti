#ifndef GAME_H
#define GAME_H

// VRAM map:
// >0000	Screen Image Table
// >0300	Sprite Descriptor Table
// >0380	Color Table
// >03A0	(Unused)
// >03C0	Color Table 2 (all white on transparent)
// >03E0	(Unused)
// >0800	Sprite Pattern Table
// >1000	Pattern table (scroll 0)
// >1800	Pattern table (scroll 2)
// >2000	Pattern table (scroll 4)
// >2800	Pattern table (scroll 6)
// >3000	Screen Image Table 2
// >3300	(unused)
// >3D00	wraparound memory overwritten by boss draw code

#define uint8 unsigned char
#define int8 signed char
#define NULL (0)
#define abs(x) ((x)<0 ? -(x) : (x))

// these functions currently live in crt0_bios.s
void *memset (void *buf, unsigned int ch, unsigned int count);
void *memcpy (void *dst, const void *src, unsigned int count);
void *memmove (void *dst, const void *src, unsigned int count);
// these don't
int strlen(const char *s);
void strcpy(char *p, const char *s);

// bank switching - nOldBank is used to let a function restore the original bank
// the banks are based on the Coleco 16k banking, thus a/b for the TI 8k banks
// our fixed bank is copied out of bank 0 a/b into 32k memory expansion
extern unsigned int nBank;
#define SWITCH_IN_PREV_BANK(nOldBank) nBank=nOldBank; (*(volatile unsigned int*)nOldBank)=nBank; 
#define SWITCH_IN_BANK0a    nBank=(unsigned int)0x6000; (*(volatile unsigned int*)0x6000)=nBank;  
#define SWITCH_IN_BANK1a    nBank=(unsigned int)0x6004; (*(volatile unsigned int*)0x6004)=nBank;  
#define SWITCH_IN_BANK2a    nBank=(unsigned int)0x6008; (*(volatile unsigned int*)0x6008)=nBank; 
#define SWITCH_IN_BANK3a    nBank=(unsigned int)0x600C; (*(volatile unsigned int*)0x600C)=nBank; 
#define SWITCH_IN_BANK4a    nBank=(unsigned int)0x6010; (*(volatile unsigned int*)0x6010)=nBank; 	
#define SWITCH_IN_BANK5a    nBank=(unsigned int)0x6014; (*(volatile unsigned int*)0x6014)=nBank; 
#define SWITCH_IN_BANK6a    nBank=(unsigned int)0x6018; (*(volatile unsigned int*)0x6018)=nBank; 
#define SWITCH_IN_BANK7a    nBank=(unsigned int)0x601C; (*(volatile unsigned int*)0x601C)=nBank; 
#define SWITCH_IN_BANK8a    nBank=(unsigned int)0x6020; (*(volatile unsigned int*)0x6020)=nBank; 
#define SWITCH_IN_BANK9a    nBank=(unsigned int)0x6024; (*(volatile unsigned int*)0x6024)=nBank; 
#define SWITCH_IN_BANK10a   nBank=(unsigned int)0x6028; (*(volatile unsigned int*)0x6028)=nBank; 
#define SWITCH_IN_BANK11a   nBank=(unsigned int)0x602C; (*(volatile unsigned int*)0x602C)=nBank; 
#define SWITCH_IN_BANK12a   nBank=(unsigned int)0x6030; (*(volatile unsigned int*)0x6030)=nBank; 
#define SWITCH_IN_BANK13a   nBank=(unsigned int)0x6034; (*(volatile unsigned int*)0x6034)=nBank; 
#define SWITCH_IN_BANK14a   nBank=(unsigned int)0x6038; (*(volatile unsigned int*)0x6038)=nBank; 
#define SWITCH_IN_BANK15a   nBank=(unsigned int)0x603C; (*(volatile unsigned int*)0x603C)=nBank; 
#define SWITCH_IN_BANK0b    nBank=(unsigned int)0x6002; (*(volatile unsigned int*)0x6002)=nBank;  
#define SWITCH_IN_BANK1b    nBank=(unsigned int)0x6006; (*(volatile unsigned int*)0x6006)=nBank;  
#define SWITCH_IN_BANK2b    nBank=(unsigned int)0x600A; (*(volatile unsigned int*)0x600A)=nBank; 
#define SWITCH_IN_BANK3b    nBank=(unsigned int)0x600E; (*(volatile unsigned int*)0x600E)=nBank; 
#define SWITCH_IN_BANK4b    nBank=(unsigned int)0x6012; (*(volatile unsigned int*)0x6012)=nBank; 	
#define SWITCH_IN_BANK5b    nBank=(unsigned int)0x6016; (*(volatile unsigned int*)0x6016)=nBank; 
#define SWITCH_IN_BANK6b    nBank=(unsigned int)0x601A; (*(volatile unsigned int*)0x601A)=nBank; 
#define SWITCH_IN_BANK7b    nBank=(unsigned int)0x601E; (*(volatile unsigned int*)0x601E)=nBank; 
#define SWITCH_IN_BANK8b    nBank=(unsigned int)0x6022; (*(volatile unsigned int*)0x6022)=nBank; 
#define SWITCH_IN_BANK9b    nBank=(unsigned int)0x6026; (*(volatile unsigned int*)0x6026)=nBank; 
#define SWITCH_IN_BANK10b   nBank=(unsigned int)0x602A; (*(volatile unsigned int*)0x602A)=nBank; 
#define SWITCH_IN_BANK11b   nBank=(unsigned int)0x602E; (*(volatile unsigned int*)0x602E)=nBank; 
#define SWITCH_IN_BANK12b   nBank=(unsigned int)0x6032; (*(volatile unsigned int*)0x6032)=nBank; 
#define SWITCH_IN_BANK13b   nBank=(unsigned int)0x6036; (*(volatile unsigned int*)0x6036)=nBank; 
#define SWITCH_IN_BANK14b   nBank=(unsigned int)0x603A; (*(volatile unsigned int*)0x603A)=nBank; 
#define SWITCH_IN_BANK15b   nBank=(unsigned int)0x603E; (*(volatile unsigned int*)0x603E)=nBank; 

#define SET_COLECO_FONT_BANK SWITCH_IN_BANK6b 

// the lib also defines gXXX variables, but these will be faster here due to constant expression elimination
#define gIMAGE 0x0000
#define gIMAGE2 0x3000
#define gSPRITES 0x0300
#define gCOLOR 0x0380
#define gCOLOR2 0x03C0
#define gSPRITE_PATTERNS 0x0800
#define gPATTERN 0x2000

// dynamic sprite tables
extern const unsigned char SPRITES[],ALTSHIELDS[],PLAYERFLAMESMALL[];
extern const unsigned char SNOWBALL[],ALTSNOWBALL[];
extern const unsigned char LADYBUG[],ALTLADYBUG[];
extern const unsigned char GNAT[],ALTGNAT[];
extern const unsigned char SELENA[],ALTSELENA[],HOMING[];

// Address of Coleco ROM font
extern const unsigned char colecofont[];

// player ships
#define SHIP_CRUISER 0
#define SHIP_SNOWBALL 1
#define SHIP_LADYBUG 2
#define SHIP_GNAT 3
#define SHIP_SELENA 4

// explosion chars 
// - char is the one that the patterns are copied into
// - copy is the one used for the SSA ship explosion
// The rest are the animation frames (must be 4)
#define EXPLOSION_FIRST 8
#define EXPLOSION_CHAR 12
#define EXPLOSION_COPY 9

// power up settings
#define PWRPULSE 0
#define PWRGNAT 3
#define PWR3WAY 4
#define PWRFRAME 0x80

// only exposing the none type for clearing
#define POWERUP_NONE	255

// player is 4 sprites
#define PLAYER_SPRITE 0

// player shots
#define NUM_SHOTS 9

// player has NUM_SHOTS shots
#define PLAYER_SHOT 22

// flame is 1 sprite
#define PLAYER_FLAME 17

// powerup is 1 sprite
#define POWERUP_SPRITE 4

// powerup colors - hand tuned to also work in F18A 3 color mode
#define POWERUP_FIRST_COLOR 2
#define POWERUP_LAST_COLOR 12
// powerup change time in frames
#define POWERUP_TIME 30

// powerups (background characters) (shield must be first, must be contiguous)
#define POWERUP_SHIELD	16
#define POWERUP_WAVE	17
#define POWERUP_3WAY	18

// shield is 4 sprites
#define PLAYER_SHIELD 18

// player ships
// function pointers are used for the init, set shield and set normal copies
// also define color 

// game flags
enum {
	MAIN_LOOP_ACTIVE,			// 0
	MAIN_LOOP_DONE,				// 1
	PLAYER_DIED,				// 2
	PLAYER_DIED_DURING_BOSS,	// 3
	BOSS_LOOP_ACTIVE,			// 4
};

// these are bitmasks for enout() to use
enum {
	DIFFICULTY_EASY = 1,
	DIFFICULTY_MEDIUM = 3,
	DIFFICULTY_HARD = 7
};

struct _sprite {
	unsigned char y, x, pat, col;
};
extern struct _sprite SpriteTab[32];

// some pointers above the stack to store data across reboots
#define SAVEDMUSIC ((unsigned int*)0x3ff6)
#define SAVEDSCORE ((unsigned char*)0x3ffa)
#define SAVEDMODE ((unsigned char*)0x3ffe)
#define SAVEDATTRACT ((unsigned char*)0x3fff)
#define SAVEDF18A ((unsigned char*)0x3ff9)

// functions
//void memset(char *p, unsigned char ch, int cnt);
void musicsync();
void spdall() ;
void loadcharset();
void patcpy(int from, int to) ;
void patsprcpy(int from, int to) ;
unsigned char rndnum();
unsigned char intpic() ;
void RLEUnpack(unsigned int p, const unsigned char *buf, unsigned int nMax);
void RLEUnpackInt(const unsigned char *bufp, const unsigned char *bufc, unsigned int nMax);
void ldpic() ;
unsigned char grf1() ;
void main() ;
void space();
void cls();
void ispace();
void sgrint();
void playmv();
void stars();
int target(unsigned int dest, unsigned int src);
void pwr(int x);
void gamovr();
void gamwin();
void scrolltext();
void gamewineasy();
void gamewinmedium();
void gamewinhard();
void DoWinMusic() ;
void mainwin();
void read();
void centr(unsigned int row, const char *out);
void scroll();
void pause();
void nmi(void);
void addscore(unsigned int val);
void getDifficulty();
void initstars();
void waitforstep();
void delaystars(unsigned int q);
void DelSprButPlayer(unsigned int x);
void background() ;
void shieldCruiser();
void shieldSnowball();
void shieldLadybug();
void shieldGnat();
void shieldSelena();
void deShieldCruiser();
void deShieldSnowball();
void deShieldLadybug();
void deShieldGnat();
void deShieldSelena();
void shieldf18();
void deshieldf18();
void handleTitlePage();
void reboot();
void noen(int x);

// macros to look like the old c99
#define screen(x) VDP_SET_REGISTER(VDP_REG_COL, x)
// write a row of characters to the screen
#define hchar(r, c, ch, cnt) vdpmemset(gIMAGE+((r)<<5)+(c), ch, cnt)
// single character hchar
#define xchar(r, c, ch) vdpchar(gIMAGE+((r)<<5)+(c), ch)
// read a character from the screen
#define gchar(r, c) vdpreadchar(gIMAGE+((r)<<5)+(c))
// set up a new sprite - (or in the magnify bit for F18A, ignored on 9918A)
#define sprite(nn, chr, color, yy, xx) SpriteTab[nn].y=yy; SpriteTab[nn].x=xx; SpriteTab[nn].pat=chr; SpriteTab[nn].col=color
// get sprite position (note: not pointers anymore)
#define spposn(nn, rr, cc) rr=SpriteTab[nn].y; cc=SpriteTab[nn].x
// set sprite location
#define sploct(nn, rr, cc) SpriteTab[nn].y=rr; SpriteTab[nn].x=cc
// set sprite color (uses assembly color - or in the magnify bit for F18A, ignored on 9918A)
#define spcolr(n,c) SpriteTab[n].col = c
// set sprite pattern
#define sppat(n,chr) SpriteTab[n].pat=chr
// delete a sprite
#define spdel(n) SpriteTab[n].y=0xd1
// set main color
#define color(nSet, nFore, nBack) vdpchar(gCOLOR+nSet, (nFore<<4)|(nBack))
#define color2(nSet, nFore, nBack) vdpchar(gCOLOR2+nSet, (nFore<<4)|(nBack))

#define SHIP_R SpriteTab[PLAYER_SPRITE].y
#define SHIP_C SpriteTab[PLAYER_SPRITE].x

// shared variables
extern unsigned int score;
extern unsigned int oldscore;
extern unsigned int scoremode;
extern unsigned int joynum;
extern int lives;
extern unsigned int level;
extern unsigned int ent[12];
extern void (*en_func[12])(int);
extern const unsigned char *pLoopMus;
extern unsigned int  loopBank;
extern unsigned int  loopIdx;
extern int ch;
extern int enr[12], enc[12];
extern int ech[12], eec[12], esc[12];
extern int ers[12], ecs[12];
extern int ep[6];		// was engine power, now general hitpoints, yes, want signed char
extern int shr[NUM_SHOTS+1], shc[NUM_SHOTS];
extern int pcr4,ptp4,pr4,pc4,p4Time;
extern int flag;
extern unsigned int nDifficulty;
extern unsigned int bgColor;
extern unsigned char tmpbuf[64];
extern const unsigned int damage[8];
extern int distns;
extern unsigned int shield;
extern unsigned int BNR,BNC;
extern unsigned int  musBank;
extern unsigned int playerColor;
extern unsigned int playerOffset;
extern unsigned int flameOffset;
extern int shotOffset;
extern unsigned int playership;
extern unsigned int playerXspeed, playerYspeed;
extern int oldshield;
extern unsigned int seed;
extern unsigned int force;
extern int f18a;


#endif

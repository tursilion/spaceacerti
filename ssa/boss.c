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

// enemy array usage:
// 0-2	engines
// 3-5	mines
// 6	cockpit (invisible collision sprite)
// 7-11	shots

//*BOSSES
// Number Rows, Number Columns
// 3 sets of Engine Row, Engine Column
// Color
const char BOSTAB[] = {
	8,11,	-8,17,	-8,49,	0,0,	COLOR_MEDGREEN,
	11,11,	-5,14,	-7,34,	-5,52,	COLOR_DKRED,
	7,11,	-8,2,	-8,34,	-8,64,	COLOR_LTBLUE,
	12,11,	-8,-1,	-2,32,	-8,66,	COLOR_DKYELLOW,
	9,15,	5,15,	6,50,	5,84,	COLOR_MAGENTA
};

// different from Coleco - we have more RAM than ROM, so copy the
// appropriate bossShape into this array - big enough to hold the largest one
unsigned int bossShape[12*2];

// a scaled difficulty level for the boss motion
uint8 scaledLevel;
#define HOMINGFRAMES 15

// all white boss palette for hit flash
const unsigned int f18WhitePalette[] = {
    0x0fff,0x0fff,0x0fff,0x0fff,0x0fff,0x0fff,0x0fff,0x0fff,
    0x0fff,0x0fff,0x0fff,0x0fff,0x0fff,0x0fff,0x0fff,0x0fff
};

char br,bd;			// these ones need to be signed
unsigned char bc;	// but this one doesn't
unsigned int BNR,BNC;
char bossminepower;
unsigned char bosscnt=0;
unsigned char enginer[3], enginec[3];   // offsets

// boss draw functions
void drawf18();

extern const unsigned int f18BlackPal[4];

// this is only called once, the boss flash is handled by changing the color table
void bosscol(char col) {
	unsigned char i;

	// set boss color
	for (i=BOSS_START/8; i<32; i++) {
		color(i,col,0);
	}
}

void boss()
{ /* boss routine */
	unsigned int i,p;
	unsigned int x_idx;
	unsigned int x_r;

	shutup();
	p=(level-1)*9;

	BNR=BOSTAB[p++];
	BNC=BOSTAB[p++];

    // LoadBossGfx below sets up the shape and draw function now

    if (f18a) {
        // set up the bml width and height registers
        VDP_SET_REGISTER(F18A_REG_BMLW, 128);   // always 128 pixels wide
    }

	// try to mask the graphics prep a little
	wrapstars();

    if (!f18a) {
    	// Boss pattern is now from BOSS_START to 255, and we reload the pattern here
	    wrapunpackboss(level);
	    wrapstars();
    }

	for (i=0; i<3; i++) {
		enginer[i]=BOSTAB[p++];
		enginec[i]=BOSTAB[p++];
        enr[i]=enginer[i];
        enc[i]=enginec[i];
	}

	// work in silence to help mask any slowdown
	shutup();

	// clear sprites 
	DelSprButPlayer(255);
	// straighten player
	wrapplayerstraight();

	// clear enemy table (except engines)
	for (i=3; i<12; i++) {
		ent[i]=ENEMY_NONE;
	}

    // no powerup either (should already be gone though)
	ptp4=POWERUP_NONE;

	// build the boss tables
    if (f18a) {
        x_r=0;
        do {
            // I think we need three wrapstars() here to keep framerate constant
            wrapstars();
            wrapLoadBossF18A(level, x_r++); // okay to call this extra times
            wrapstars();        // need to do it twice to keep speed steady
            wrapplayer();
            wrapstars();        // calls waitforstep()
        } while (wrapLoadBossF18A(level, x_r++));   // should be quick enough
    } else {
	    for (x_idx=0; x_idx<3; x_idx++) {
		    for (x_r=0; x_r<BNR; x_r++) {
			    wrapplayer();
			    wrapstars();	// calls waitforstep()

			    PrepareBoss(x_idx, x_r);
			    wrapstars();	// calls waitforstep()
		    }
	    }
    }

	// set up the scaled movement speed
	scaledLevel = level;
	if (scaledLevel > 3) scaledLevel = 3;	// max out boss movement speed
	if ((nDifficulty == DIFFICULTY_EASY)&&(scaledLevel > 2)) scaledLevel=2;

	// copy in the homing graphics overtop of the beam left
    wrapLoadBossGfx();

	// start the music now
	StartMusic(BOSSMUS, 1);
	
	// erase the 'boss approaching' text
	hchar(11, 0, 32, 32);

	// setup boss
	bossminepower=10;	// starts the same as in a stage, but gets harder FAST
	bc=0; bd=0;
	br=-BNR;
	if (scoremode == 3) {
		// invisible enemies
		if (!f18a) {
            bosscol(COLOR_BLACK);
        }
	} else {
		bosscol(BOSTAB[p]);
	}

	// cockpit: we set this up now, though it will be partially up again when the boss is fully onscreen
	// otherwise the enemy code crashes trying to process the enemy function
	ent[6]=ENEMY_NONE;		// not active yet however!
	en_func[6]=enemynull;	// an enemy who does nothing (I suppose the movement could go in there for consistency, but.. nah)
	ers[6]=0;	// ers and ecs no longer used, but we'll clear them
	ecs[6]=0;
	enr[6]=(BNR<<3)-32; 
	enc[6]=(BNC<<2)-12;	// <<3>>1 == <<2
	//sprite(6+ENEMY_SPRITE, 228, 0, enr[6], enc[6]); // it doesn't actually have to be on screen!

	// setup rest of boss's engines
	for (i=0; i<3; i++)	{
		if ((enr[i]==0)&&(enc[i]==0)) {
			// not defined, skip it
			ent[i]=ENEMY_NONE;
			ep[i]=0;
			continue;
		}
		ech[i]=76; esc[i]=76; eec[i]=80;
		ent[i]=ENEMY_ENGINE; ers[i]=0; ecs[i]=0;
		en_func[i]=enemyengine;     // just animates
		ep[i]=level*5;		// hit points (engine power)
		sprite(i+ENEMY_SPRITE,76,f18a?PAL_BOSSFLAME:COLOR_MEDRED,enr[i],enc[i]);
	}

	// load up the engine sprites
	wrapLoadEngineSprites();

	// exit this loop when someone dies!
	flag=BOSS_LOOP_ACTIVE;
	while (flag == BOSS_LOOP_ACTIVE) {
		// frame 0
		wrapplayer();
		wrapcolchk(1);
		wrapstars();

		// frame 1
		mboss();
		wrapcolchk(0);
		wrapstars();

		// frame 2
		wrapenemy();

		if (ch) {
			wrapcheat();
		} else {
			wrapplycol();
			// oi.. this generic 'flag' system sucks....
			if (flag == MAIN_LOOP_ACTIVE) {
				// roll back a bit to repeat the boss battle
				flag=PLAYER_DIED_DURING_BOSS;
			}
		}

		whoded();
		if ((ep[0]==0)&&(ep[1]==0)&&(ep[2]==0)) {
			flag=MAIN_LOOP_ACTIVE;
		}
		wrapstars();

		if (bosscnt) {
			if (--bosscnt == 0) {
				// reset boss color set
                if (f18a) {
                    wrapLoadBossF18A(level, 99);    // force palette reload
                } else {
                    VDP_SET_REGISTER(VDP_REG_CT, 0x0e); // CT at >0380
                }
			}
		}
    }

	// reset boss color set in case it flashed (just always)
    if (f18a) {
        wrapLoadBossF18A(level, 99);    // force palette reload
    } else {
        VDP_SET_REGISTER(VDP_REG_CT, 0x0e); // CT at >0380
    }
	
	// check if player won the battle
	if (flag == MAIN_LOOP_ACTIVE) {
        if (joynum) {
	    	byboss();
        } else {
            // didn't really, but this will end the demo
            flag = PLAYER_DIED_DURING_BOSS;
        }
	}

    // make sure it's turned off and palette restored in case it's F18A
    erboss();

    // restore the beam graphics
    wrapRestoreBossGfx();
}

// sadly we are out of vblank before this function is done
void drboss() { 
	/*draw boss ship*/
	unsigned char ch=BOSS_START;		// boss chars run from BOSS_START-255
	unsigned char tc,to;
	unsigned char roff,coff;

    tc=bc>>2;
	to=bc&0x03;
	
    if (f18a) {
        // position the bitmap overlay
        // br is a character row for the boss (negative boss height to 1, I think)
        // bc is a fat pixel column (0-127), I believe. it might be 2 pixels as we divide by 4 above
        if (br < 0) {
            // partially offscreen - adjust the pattern - every vertical step is 2 pixel rows, and br is negative
            VDP_SET_REGISTER(F18A_REG_BMLADR, 160-(br*4));  // remember BR is negative
            VDP_SET_REGISTER(F18A_REG_BMLH, BNR*8+(br*8));  // remember BR is negative
            VDP_SET_REGISTER(F18A_REG_BMLX, bc*2);    // directly?
            VDP_SET_REGISTER(F18A_REG_BMLY, 0);     // locked to the top row
        } else {
            // fully onscreen - just position it
            VDP_SET_REGISTER(F18A_REG_BMLADR, 160);  // make sure base address is right
            VDP_SET_REGISTER(F18A_REG_BMLH, BNR*8);  // remember BR is negative
            VDP_SET_REGISTER(F18A_REG_BMLX, bc*2);   // directly?
            VDP_SET_REGISTER(F18A_REG_BMLY, br*8+3);   // I think we move down one
        }
        // and set the control register
        VDP_SET_REGISTER(F18A_REG_BMLCFG, 0xf4);    // enable, over tiles, transparent, fat pixels, palette 4
    } else {
    	// update pattern table for scroll pos (0-3 subpixels)
        VDP_SET_REGISTER(VDP_REG_PDT, 4+to);
	    // handle the inline draw function
	    wrapbossdraw();
    }

	// engines (lowest offset is -8, highest is 20)
	roff=br<<3;
	// make sure offscreen engines don't wrap around to the bottom (192 is bottom)
	// but don't allow 208 (which would blank the rest of the sprite table)
	if (roff > 127) roff=200;	// chosen because all row values end up off the bottom but never equal 208
	coff=(bc<<1)+bd;
	if (ep[0]) {
        enr[0] = roff+enginer[0];
        enc[0] = coff+enginec[0];
		sploct(ENEMY_SPRITE,enr[0],enc[0]);
	}
	if (ep[1]) {
        enr[1] = roff+enginer[1];
        enc[1] = coff+enginec[1];
		sploct(1+ENEMY_SPRITE,enr[1],enc[1]);
	}
	if (ep[2]) {
        enr[2] = roff+enginer[2];
        enc[2] = coff+enginec[2];
		sploct(2+ENEMY_SPRITE,enr[2],enc[2]);
	}

	// move cockpit 'bullet' as close to player as we are allowed to go (so if they overwrite us, we get them)
    // there are some minor bugs when boss is near the edges that cause incorrect lookups, but I don't think it's
    // exploitable... and I don't care if it is. More power to you!
	enr[6] = SHIP_R;
	if (enr[6] > ((BNR-1)<<3)) enr[6]=((BNR-1)<<3);
	enc[6] = SHIP_C;
    {
        unsigned char bossr = ((enr[6])>>3)<<1;
        if (enc[6] < coff+bossShape[bossr]) enc[6] = coff+bossShape[bossr];
        else if (enc[6] > coff+bossShape[bossr+1]) enc[6] = coff+bossShape[bossr+1];
    }

	if (br >= 1) {
		// some events delete bullets, so just add it back (if it's active)
		ent[6]=ENEMY_SHOT;
	}
}
 
void erboss() {
	/*erase boss*/
	unsigned char i;
	unsigned int p;

    if (f18a) {
        // just turn it off
        VDP_SET_REGISTER(F18A_REG_BMLCFG, 0);
        // and restore the palette for that section of sprites
        // don't reload the palette if we are playing cloaked ships though
        if (scoremode!=3) {
            wrapLoadF18MainPalette();
        }
    } else {
	    p=gIMAGE+(bc>>2)+(br<<5);

	    for (i=0; i<BNR; i++) {
		    vdpmemset(p, 32, BNC);
		    p+=32;
	    }
    }
}

void mboss() { 
	/*boss control*/
	static unsigned char next=32;
	uint8 x,a;

	// get the update out of the way first
	bc=bc+bd;
	if (br < 1) {
		br++;
		if (br == 1) {
            if (!f18a) {
			    // clear the top row and the left most edge, some of the
			    // bosses leave a little trail. hopefully the one-frame hiccup
			    // is forgivable :)
			    hchar(0, 0, 32, 32);
			    hchar(1, 0, 32, 12);
			    hchar(2, 0, 32, 12);
			    hchar(3, 0, 32, 12);

			    // now put the score back too ;)
			    addscore(0);
            }

			// activate the cockpit sprite
			ent[6] = ENEMY_SHOT;
		}
	}
	drboss();

	// now fix the direction for next time
	x=scaledLevel;
	if (bc>=(31-BNC)<<2) bd=-x;
	if (bc<=1) bd=x;

	if (level == 1) {
		if ((ent[8]==ENEMY_NONE) && ((rndnum()&7)<3)) { 
			ent[7]=ENEMY_SHOT; ent[8]=ENEMY_SHOT;
			en_func[7]=enemyshot; en_func[8]=enemyshot;
			enr[7]=56+(br<<3); enr[8]=enr[7];
			enc[7]=(bc<<1)+16; enc[8]=enc[7]+40;
			ecs[7]=0; ecs[8]=0;
			ers[7]=3+scaledLevel; ers[8]=ers[7];
			sprite(ENEMY_SPRITE+6+1,84,f18a?PAL_SHOT:COLOR_DKGREEN,enr[7],enc[7]);
			sprite(ENEMY_SPRITE+6+2,84,f18a?PAL_SHOT:COLOR_DKGREEN,enr[8],enc[8]);
		}
	} else if (level == 2) {
		if ((ent[8]==ENEMY_NONE) && ((rndnum()&7)<3)) { 
			ecs[7]=-3; ecs[8]=0; ecs[9]=3;
			for (a=7; a<10; a++) { 
				if ((nDifficulty == DIFFICULTY_EASY) && (a != 8)) {
					continue;
				}
				ent[a]=ENEMY_SHOT; 
				en_func[a]=enemyshot;
				enr[a]=80+(br<<3);
				enr[8]=enr[a]+8;		// no need to do a conditional, just update it each time
				enc[a]=(bc<<1)+36;
				ers[7]=3+scaledLevel; ers[8]=ers[7]; ers[9]=ers[7];
				if (nDifficulty == DIFFICULTY_MEDIUM) {
					// less steep angle
					ecs[7]=-2;
					ecs[10]=2;
				}
				sprite(a+ENEMY_SPRITE,84,f18a?PAL_SHOT:COLOR_DKGREEN,enr[a],enc[a]); 
			}
		}
	} else if (level==3) { 
		// decide if mines come out
		if ((rndnum()&7)<3) {
			x=255;
			if (ent[3] == ENEMY_NONE) {
				x=3;
			}
			if ((nDifficulty >= DIFFICULTY_MEDIUM) && (ent[4] == ENEMY_NONE)) {
				x=4;
			}
			if ((nDifficulty == DIFFICULTY_HARD) && (ent[5] == ENEMY_NONE)) {
				x=5;
			}
			if (x<255) { 
				ent[x]=ENEMY_MINE;
				en_func[x]=enemymine;
				ep[x]=bossminepower++;	// muhaha - harder than in the levels, and worse the longer you stay
				if (bossminepower > 126) bossminepower=126;
				enr[x]=64+(br<<3);
				enc[x]=(bc<<1)+45;
				eec[x]=8; esc[x]=8; ech[x]=8;
				if (scoremode == 3) {
					// invisible enemies
   					sprite(x+ENEMY_SPRITE,8,bgColor,enr[x],enc[x]);
				} else {
					sprite(x+ENEMY_SPRITE,8,f18a?PAL_MINE:COLOR_CYAN,enr[x],enc[x]);
				}
			}
		}
	} else if (level == 4) {
		// staggered homing shots from around 35,71
		if ((next>=0x50)&&((VDP_INT_COUNTER&0x7f)<32)) {
			next=0x10;
		}
		if ((VDP_INT_COUNTER&0x7f) >= next) {
			unsigned char ok=0;
			switch(next&0xf0) {
				case 0x10:	// only hard
					if (nDifficulty==DIFFICULTY_HARD) ok=1;
					break;
				case 0x30:	// medium or hard
					if (nDifficulty!=DIFFICULTY_EASY) ok=1;
					break;
				case 0x50:	// all of them
					ok=1;
					break;
			}
			next+=0x10;
			if (ok) {
				// find a free shot - maximum 3!
				for (unsigned char a=7; a<10; a++) { 
					if (ent[a]==ENEMY_NONE) {
						ers[a]=0;	// start static
						ecs[a]=0;
						ent[a]=ENEMY_SHOT; 
						en_func[a]=enemyhoming;
						ech[a]=HOMINGFRAMES+nDifficulty;    // how many frames we can home for (difficulty adds 1, 3, 7 frames)
						enr[a]=71+(br<<3); 
						enc[a]=(bc<<1)+35;
						sprite(a+ENEMY_SPRITE,104,f18a?PAL_BOSSHOMETIP:COLOR_WHITE,enr[a],enc[a]);
						break;
					}
				}
			}
		}
	} else if (level == 5) {
		// three phases - drop one mine, fire bullets, or launch up to 3 homing shots
		if ((next>=0xf0)&&((VDP_INT_COUNTER&0x3f)<32)) {
			next=16;
		}
		if (VDP_INT_COUNTER >= next) {
			// we step by 16s and space out our actions - note because of previous
			// bosses, we might start at any pattern
			// We assume the shots don't conflict for performance, they usually shouldn't...
			switch (next&0xf0) {
				case 0x20:
					// fire guns
					for (a=7; a<12; a++) {
						noen(a);	// make sure any lingering homing lasers are killed
						if ((nDifficulty < DIFFICULTY_HARD) && ((a==7)||(a==11))) {
							continue;
						}
						if ((nDifficulty == DIFFICULTY_EASY) && (a==9)) {
							continue;
						}
						ent[a]=ENEMY_SHOT; 
						en_func[a]=enemyshot;
						switch(a) {
							case 7:
								enr[7]=56+(br<<3);
								enc[7]=(bc<<1)+32;
								ecs[7]=-8;
								ers[7]=3+scaledLevel;
								break;
							case 8:
								enr[8]=64+(br<<3);
								enc[8]=(bc<<1)+40;
								ecs[8]=-4;
								ers[8]=3+scaledLevel;
								break;
							case 9:
								enr[9]=72+(br<<3);
								enc[9]=(bc<<1)+48;
								ecs[9]=0;
								ers[9]=3+scaledLevel;
								break;
							case 10:
								enr[10]=64+(br<<3);
								enc[10]=(bc<<1)+64;
								ecs[10]=4;
								ers[10]=3+scaledLevel;
								break;
							case 11:
								enr[11]=56+(br<<3);
								enc[11]=(bc<<1)+72;
								ecs[11]=8;
								ers[11]=3+scaledLevel;
								break;
						}
						sprite(a+ENEMY_SPRITE,84,f18a?PAL_SHOT:COLOR_DKGREEN,enr[a],enc[a]);
					}
					break;

				case 0x40:
				{
					// drop one mine
					unsigned char x=255;
					if (ent[3] == ENEMY_NONE) {
						x=3;
					}
					if ((nDifficulty >= DIFFICULTY_MEDIUM) && (ent[4] == ENEMY_NONE)) {
						x=4;
					}
					if ((nDifficulty == DIFFICULTY_HARD) && (ent[5] == ENEMY_NONE)) {
						x=5;
					}
					if (x<255) { 
						ent[x]=ENEMY_MINE;
						en_func[x]=enemymine;
						ep[x]=bossminepower++;	// muhaha - harder than in the levels, and worse the longer you stay
						if (bossminepower > 126) bossminepower=126;
						enr[x]=64+(br<<3);
						enc[x]=(bc<<1)+45;
						eec[x]=8; esc[x]=8; ech[x]=8;
						if (scoremode == 3) {
							// invisible enemies
   							sprite(x+ENEMY_SPRITE,8,bgColor,enr[x],enc[x]);
						} else {
							sprite(x+ENEMY_SPRITE,8,f18a?PAL_MINE:COLOR_CYAN,enr[x],enc[x]);
						}
					}
				}
					break;

				case 0x80:
                    // don't fire homing laser if we are not fully onscreen
                    if (br >= 1) {
					    // homing laser shot - we assume the bullets are all free!
					    ers[7]=0;	// start static
					    ecs[7]=0;
					    ent[7]=ENEMY_SHOT; 
					    en_func[7]=enemyhominglaser;	// ONLY 7 is legal!
					    ech[7]=HOMINGFRAMES+HOMINGFRAMES;
					    enr[7]=50+(br<<3); 
					    enc[7]=(bc<<1)+48;
					    sprite(7+ENEMY_SPRITE,104,f18a?PAL_BOSSHOMETIP:COLOR_WHITE,enr[7],enc[7]);
					    for (unsigned char a=8; a<12; ++a) {
						    ers[a]=0;	// start static
						    ecs[a]=0;
						    ent[a]=ENEMY_EXPLOSION;	// no collision, just a trail
						    en_func[a]=enemynull;
						    enr[a]=enr[7]; 
						    enc[a]=enc[7];
                            if (f18a) {
    						    sprite(a+ENEMY_SPRITE,104,a<10?PAL_BOSSHOMELT:PAL_BOSSHOMEDK,enr[a],enc[a]);
                            } else {
    						    sprite(a+ENEMY_SPRITE,104,a<10?COLOR_LTBLUE:COLOR_DKBLUE,enr[a],enc[a]);
                            }
					    }
                    }
					break;
			}
			next+=16;
		}
	}
}

int checkdamage(int sr, int sc, int pwr) {
	int b;
	int rd,cd;
	unsigned int p;

	rd=sr>>3;
	if (rd <= br+BNR) {
		cd=(sc+4)>>3;
        if (f18a) {
            // okay, so determine the address of the overlay byte under the sprite position
            if (br > 0) {
                // only worry about it once the boss is fully onscreen - this may be a slight difference?
                // the damage tracking is gonna be different anyway cause of the pixel size
                sr=rd<<3;   // lock the row to a multiple of 8
                sc=cd<<3;   // column too (if we don't, the check at p+4 rows won't be reliable)
                sr-=br*8;   // boss row is in character rows
                sc-=bc*2;   // boss column is in fat pixels

                // they are unsigned chars, so there's no negative, just check for off the edge of the bitmap
                // we can use the existing data for boss width and height
                if ((sr < BNR*8)&&(sc < BNC*8)) {
                    // okay, we are within the rectangle, get an address
                    // every row is 32 bytes (always), and every byte contains 4 pixels
                    p = sr*(128/4) + sc/4 + BOSS_PATTERN;
                    
                    // check if it's got anything - we'll use the same sort of check as the other one does
                    // we just don't need to look up the character separately. Plus 4 there, though, is plus 
                    // 128 here, cause it's 32 bytes per row.
                    b = vdpreadchar(p+128);
                    if (b) {
				        // this block is solid, nuke it
				        // this code is similar but not identical to the non-F18A version
				        AddDamageF18a(p);
				        addscore(1);
				        if (pwr==PWRPULSE+2) {
					        // maximum pulse shot does double damage
					        sc += 8;
                            if (sc < BNC*8) {
						        // repeat for the next char
                                p += 2;     // every byte is 4 pixels
						        AddDamageF18a(p);
						        addscore(1);
					        }
				        }
				        return 1;	// only one shot per frame hits the boss body (for performance's sake)
                    }
			    }
            }

        } else {

		    b=gchar(rd,cd);
		    if (b>=BOSS_START) {
			    // potential - check the character pattern
			    p=(b<<3)+gPATTERN;
			    b = vdpreadchar(p+4);
			    if (b) {
				    // this block is solid, nuke it
				    AddDamage(p);
				    addscore(1);
				    if (pwr==PWRPULSE+2) {
					    // maximum pulse shot does double damage
					    cd++;
					    b=gchar(rd,cd);		// read new char, make sure it's really there
					    if (b>=BOSS_START) {
						    // repeat for the next char
						    p=(b<<3)+gPATTERN;
						    AddDamage(p);
						    addscore(1);
					    }
				    }
				    return 1;	// only one shot per frame hits the boss body (for performance's sake)
			    }
		    }
        }
	}
	return 0;
}
 
void whoded() { 
	int rd,cd;
    int r,c;

	/*check boss specific collisions*/
	for (int a=0; a<NUM_SHOTS; a++) {
		// check for valid shot
		if (!shr[a]) continue;

		// check if hit a piece of boss
		if (checkdamage(shr[a], shc[a], pwrlvl&0x0f)) {
            playsfx_hitboss();
			spdel(a+PLAYER_SHOT);
			shr[a]=0;
            continue;
		}

		// check if hit an engine
        for (int b=0; b<3; ++b) {
            if (ent[b] != ENEMY_ENGINE) continue;
            spposn(b+ENEMY_SPRITE, r, c);
            rd = abs(r-shr[a]);
            if (rd <= 20) {
                cd = abs(c-shc[a]);
                if (cd <= 15) {
                    if (f18a) {
                        loadpal_f18a(f18WhitePalette, PAL_BOSS*4, 16);  // set boss palette to white
                    } else {
                        VDP_SET_REGISTER(VDP_REG_CT, 0x0f); // CT at >03C0 (makes boss flash white, everything else already is)
                    }
					bosscnt=3;						// how many cycles to stay white (should be 3 frames per cycle)
					ep[b]-=damage[pwrlvl&0x07];
					if (ep[b]<=0) { 
                        playsfx_explosion();
						addscore(5); 
						ep[b]=0; 
						enr[b]=192; 
                        ent[b]=ENEMY_NONE;
						spdel(b+ENEMY_SPRITE);
					} else {
                        playsfx_armor();
                    }
					spdel(a+PLAYER_SHOT); 
					shr[a]=0; 
					break;
				}
			}
		}
	}
}

// I want to use color 4, rather than 1, so copy to the third table (F18A only)
void bosssprcpy(uint8 from, uint8 to) {
	vdpmemread((from<<3)+gPATTERN, tmpbuf, 8);
	vdpmemcpy((to<<3)+gSPRITE_PATTERNS+0x1000, tmpbuf, 8);
}

void byboss() { 
	/*boss is dead...blow him up!*/
	int tmp, qw;
	uint8 a, x;
	unsigned char tc;
	unsigned char r,c;

	tc=bc>>2;

	shutup();
	// erase enemies and shots
	DelSprButPlayer(PLAYER_FLAME);
	playmv();			// redraw the shield, DelSprButPlayer will have erased it

	// straighten player
	wrapplayerstraight();
    wrapPlayerFlameSmall();

    // for F18a, we need to copy over the explosion character into the sprite table (just the first for now)
    if (f18a) {
        // use the beam left/boss homing sprite - we need to convert from character to sprite though
        bosssprcpy(EXPLOSION_FIRST,104);
        // because we're F18A, erase the rest
        vdpmemset(104*8+0x1808, 0, 24);     // rest of third table
        vdpmemset(104*8+0x1000, 0, 32);     // all of second table
        vdpmemset(104*8+0x0800, 0, 32);     // all of first table
        // we're going to use tmpbuf[64] to remember where the sprites are
        memset(tmpbuf, 0, sizeof(tmpbuf));
    }

	// draw explosions over boss
	a=0;
	for (qw=0; qw<290; qw++) { 
		x=qw/20;
		SOUND=0xe5;
		SOUND=0x89+(qw&0xf);
		SOUND=0x3f;
		SOUND=0xa0+(qw&0xf);
		SOUND=0x3f;
		SOUND=0xc7+(qw&0xf);
		SOUND=0x3e;
		SOUND=0x90+x;
		SOUND=0xb0+x;
		SOUND=0xd0+x;
		SOUND=0xf0+x;

        if (f18a) {
            unsigned char sr, sc, sp;
            unsigned int p;
            // We could do the explosion chars as sprites (we can use 4-16 and 22-31 for a total of 23 which isn't bad)
		    r=(rndnum()>>2)%BNR; 
            c=(rndnum()>>2)%BNC;
            // to determine if it's legal we'll use similar to the hit detection, but at offset 0
            sr=r<<3;   // lock the row to a multiple of 8
            sc=c<<3;   // column too (if we don't, the check at p+4 rows won't be reliable)

            // first check if we already did this position, and return a valid sprite index
            sp=32;  // out of range
            for (unsigned char i=4; i<32; ++i) {
                if (i == 17) i=22;
                if ((tmpbuf[i*2] == sr)&&(tmpbuf[i*2+1] == sc)) {
                    sp=i;
                    break;
                }
                if (SpriteTab[i].y == 0xd1) {
                    sp = i; // remember free sprite
                }
            }

            // if we didn't find a valid sprite, just ignore this cycle
            if (sp < 32) {
                // we have a sprite to work with
                if (SpriteTab[sp].y == 0xd1) {
                    // using the bossShape table, see if we can contain the explosion sprites
                    // we can assume row is acceptable.
                    {
                        unsigned char bossr = r<<1;
                        if (((c<<3) >= bossShape[bossr])&&((c<<3) <= bossShape[bossr+1]+1)) {
                            // palette 8 is flames/explosions - this is the f18a only mode
                            sprite(sp, 104, PAL_EXPLODE, (r<<3)+br*8, (c<<3)+bc*2+4);
                            tmpbuf[sp*2]=r<<3;
                            tmpbuf[sp*2+1]=c<<3;
                        }
                    }
                } else {
                    // this was already a valid sprite, so remove it
                    spdel(sp);
                    tmpbuf[sp*2]=0xd1;

                    // okay, we are within the rectangle, get an address
                    // every row is 32 bytes (always), and every byte contains 4 pixels
                    p = sr*(128/4) + sc/4 + BOSS_PATTERN;
                    AddDestroyedF18a(p);
		        }
            }
        } else {
		    // draw random explosion cell (or erase old one!)
		    r=(rndnum()>>2)%BNR; 
            c=(rndnum()>>2)%BNC;
		    x=gchar(br+r,tc+c);
		    if (x>127) {
			    if (++a > 1) {
				    xchar(br+r,tc+c,EXPLOSION_CHAR);		// draw explosion shape if boss
				    a=0;
			    }
		    } else if (x==EXPLOSION_CHAR) {
			    x=r*BNC+c+BOSS_START;					    // draw blank if already explosion shape - this gets correct char in the boss pattern
			    wrapAddDestroyed((x<<3)+gPATTERN);          // this wipes the char in all tables
			    xchar(br+r, tc+c, x);                       // and this replaces the explosion char with the original (now blank)
		    }
        }

		// animate stars
		wrapstars();
		
		if ((qw&3)==1) {
			// shake boss back and forth (first and last table only - max shake of 6 pixels)
			if (f18a) {
                VDP_SET_REGISTER(F18A_REG_BMLX, ((qw>>1)&0x02) ? bc*2 : bc*2+6);
                // animate sprite version of explosions
                bosssprcpy(EXPLOSION_FIRST+((qw>>2)&3),104);
            } else {
                VDP_SET_REGISTER(VDP_REG_PDT, 4+((qw>>1)&0x2));
			    // animate explosions
			    patcpy(EXPLOSION_FIRST+((qw>>2)&3), EXPLOSION_CHAR);
            }
		}
	}
	// erase boss and delay
	DelSprButPlayer(PLAYER_FLAME);
	erboss();
	shutup();

	// set pattern table back to default >2000
    VDP_SET_REGISTER(VDP_REG_PDT, 4);

	// boss destroyed announcement
	centr(11, "BOSS DESTROYED BONUS");
	tmp=100;
	if (nDifficulty >= DIFFICULTY_MEDIUM) tmp+=50;
	if (nDifficulty == DIFFICULTY_HARD) tmp+=50;

	for (qw=tmp; qw>=0; qw--) {
		int x = qw;
		VDP_SET_ADDRESS_WRITE((int)(gIMAGE+384+13));
		VDPWD=((x/100)+'0');
		x%=100;
		VDPWD=((x/10)+'0');
		VDPWD=((x%10)+'0');
		VDP_SAFE_DELAY();
		VDPWD=('0');
		VDP_SAFE_DELAY();
		if (qw == tmp) {		// interleaving for VDP writes
			VDPWD=('0');
			delaystars(30);
		} else {
			VDPWD=('0');
			delaystars(1);
			addscore(1);
		}
	}

	delaystars(10);

    if (force) {
        centr(13, "10000 FOR USING THE FORCE");
        addscore(100);
       	delaystars(20);
    }

    // check for end of game bonus
    if ((level == 5)&&(nDifficulty > DIFFICULTY_EASY)) {
        color(15, COLOR_WHITE, COLOR_TRANS);  // so the 'x' shows up
        if (playership == SHIP_SELENA) {
            centr(14, "10000 BEST PRINCESS BONUS");
            addscore(100);
            delaystars(120);
        } else if ((playership == SHIP_SNOWBALL)||(playership==SHIP_LADYBUG)||(playership==SHIP_GNAT)) {
            centr(14, "SPARE LIVES BONUS 0x10000");
            for (x=0; x<=lives; ++x) {
                delaystars(30);
                vdpchar((int)(gIMAGE+448+21), x+'0');
                vdpchar((int)(gIMAGE+738+(lives-x)), ' ');
                addscore(100);
            }
            delaystars(120);
        } else if (playership == SHIP_CRUISER) {
            // 0,25,50,75 are only valid values
            x=0;
            centr(14, "SPARE SHIELD BONUS 0x10000");
            while (shield >= 25) {
                delaystars(30);
                ++x;
                vdpchar((int)(gIMAGE+448+21), x+'0');
                vdpchar((int)(gIMAGE+738+(lives-x)), ' ');
                addscore(100);
                shield -= 25;
                playmv();   // force a shield image update
            }
            delaystars(120);
        }
        if (scoremode == 3) {
            // cloaked enemies
            centr(15,"INCREDIBLE CLOAKED BONUS 20000");
            addscore(200);
            delaystars(60);
        }
    }

    // fly off screen
    wrapwarpout();
	flag=MAIN_LOOP_ACTIVE;
}
 
// damage pattern for smooth horizontal scrolling for the boss
void AddDamage(unsigned int ptr) {
 	// add 8 bytes of random noise at ptr, but we
 	// also need to shift it through the other 3 tables
 	unsigned short *mask;	// needs 16 bytes
 	unsigned char idx,idx2;

	// Tried unrolling and making this explicit - but it was exactly
	// the same speed, even over 15,000 iterations. So left the loops.

	// alias mask into tmpbuf, which is 64 bytes
	mask=(unsigned short*)&tmpbuf[16];

	// do initial pattern
	vdpmemread(ptr, tmpbuf, 8);
	tmpbuf[4]=0;

	for (idx=0; idx<8; idx++) {
 		mask[idx]=rndnum()&0xff;
 		tmpbuf[idx]&=mask[idx];
		mask[idx]<<=8;
		mask[idx]|=0xff;
 	}
 	mask[4]=0x00ff;				// test line, wipe it

 	vdpmemcpy(ptr, tmpbuf, 8);

	// do the rest of the patterns
	for (idx=0; idx<3; idx++) {
 		ptr+=SCROLL_OFFSET;				// offset is to the next table
 		vdpmemread(ptr, tmpbuf, 16);
 		for (idx2=0; idx2<8; idx2++) {
 			mask[idx2]>>=2;		// shift 2 pixels right
 			mask[idx2]|=0xc000;	// preserve shifted in pixels
 			tmpbuf[idx2]&=(mask[idx2]>>8)&0xff;
 			tmpbuf[idx2+8]&=mask[idx2]&0xff;
 		}
 		vdpmemcpy(ptr,tmpbuf,16);
 	}
}
void AddDamageF18a(unsigned int ptr) {
    // we have a GPU program to do this
    VDP_SET_ADDRESS_WRITE(GPU_DAMAGEIN);
    VDPWD = (ptr>>8);   // no need for delays either!
    VDPWD = (ptr&0xff);
    VDPWD = 0;          // no data
    VDPWD = 0;
    VDPWD = 0;
    VDPWD = 1;          // command to go
}

void AddDestroyedF18a(unsigned int ptr) {
    // we have a GPU program to do this
    VDP_SET_ADDRESS_WRITE(GPU_DAMAGEIN);
    VDPWD = (ptr>>8);   // no need for delays either!
    VDPWD = (ptr&0xff);
    VDPWD = 0;          // no data
    VDPWD = 0;
    VDPWD = 0;
    VDPWD = 3;          // command to go
}

// copies the boss patterns scrolled into the next cell
// idx indicates the target scroll table (0-3)
// r indicates the boss row (0-(BNR-1))
// Not used by F18A
void PrepareBoss(unsigned int idx, unsigned int r) {
	unsigned char idx2;
	char c;
	unsigned int p,basep;

	// get start address (last two characters of first row)
	basep=((BOSS_START+BNC-2)<<3)+gPATTERN;

	// calculate the offset for this pass
	for (idx2=0; idx2<idx; idx2++) {
		basep+=SCROLL_OFFSET;
	}
	p=basep;
	for (idx2=0; idx2<(unsigned)r; idx2++) {
		p+=BNC<<3;
	}

	// this seems okay for time now. If not, break up the columns
	// using any interrupt method other than waitforpoll can screw
	// up the timing for the sprite copy and cause corruption
	
	for (c=BNC-1; c>0; c--) {
		vdpmemread(p, tmpbuf, 16);	// read character and its neighbor
		// we need to shift 2 pixels from the neighbor for each row
		for (idx2=0; idx2<8; idx2++) {
			// first, shift ourselves (discard the loss)
			tmpbuf[8+idx2]>>=2;
			// next, get the two LSBs from the neighbor and drop them in place
			tmpbuf[8+idx2]|=(tmpbuf[idx2]&0x03)<<6;
		}
		// now only write ourselves back, but in the next table
		vdpmemcpy(p+8+SCROLL_OFFSET, tmpbuf+8, 8);
		// except if column 0 was involved (saves an extra read/write)
		if (c == 1) {
			for (idx2=0; idx2<8; idx2++) {
				// just need to shift, nothing new to come in
				tmpbuf[idx2]>>=2;
			}
			vdpmemcpy(p+SCROLL_OFFSET, tmpbuf, 8);
		}
		// decrement the pointer
		p-=8;
	}
}

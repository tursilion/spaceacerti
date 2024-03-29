#include <grom.h>
#include <vdp.h>
#include <kscan.h>
#include "game.h"
#include "highscores.h"
#include "trampoline.h"
#include "music.h"

extern void displayScore();
extern void checkQuit();

// duplicate data to save paging
static const unsigned char LCARS[] = {
    0x00,0x00,0x0F,0x30,0x63,0x67,0x66,0x66,
    0x00,0x00,0xFF,0x00,0xFF,0x00,0x00,0x00,
    0x00,0x00,0xF0,0x0C,0xC6,0xE6,0x66,0x66,
    0x66,0x66,0x67,0x63,0x30,0x0F,0x00,0x00,
    0x66,0x66,0xE6,0xC6,0x0C,0xF0,0x00,0x00,
    0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,
    0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,0x00
};

// write the high scores out - we assume that the device was already checked for
void saveScores(struct _scores *scores) {
    // unlock the eeprom
	GromWriteData(0xffff, 15, 0x55);
	GromWriteData(0xffff, 15, 0xaa);
	GromWriteData(0xffff, 15, 0x5a);

    if (scores != NULL) {
        // now write the data
        unsigned char *pDat = (unsigned char*)scores;
        for (int idx=UBERGROM_WRITE; idx<UBERGROM_WRITE+sizeof(struct _scores); ++idx) {
            // A bit slower this way, setting the address each time
            GromWriteData(idx, 15, *(pDat++));
        }
    }

    // and write out the music setting as 1-4
    if (*SAVEDMUSIC == (unsigned int)doSfxInstead) {
        GromWriteData(UBERGROM_MUSIC, 15, '4');
    } else if (*SAVEDMUSIC == (unsigned int)doMusicOnly) {
        GromWriteData(UBERGROM_MUSIC, 15, '3');
    } else if (*SAVEDMUSIC == (unsigned int)doForTI) {
        GromWriteData(UBERGROM_MUSIC, 15, '2');
    } else {
        GromWriteData(UBERGROM_MUSIC, 15, '1');
    }

    // and relock the eeprom
	GromWriteData(0xffff, 15, 0);
}

// return true if there's a high score ubergrom attached
int checkHighScores() {
    // just check the configuration bits. If bytes 0 and 1
    // are not inverted copies of each other, assume no ubergrom
    // config space is always mapped, so we can just go ahead and read
    GROM_SET_ADDRESS(UBERGROM_CHECK);
    unsigned char a = UBERGROM_RD;
    unsigned char b = UBERGROM_RD;
    // this is really important. If we do all this without casting, then (~b) is promoted
    // to an unsigned int (with the MSB set to 00). This forces a to be promoted as
    // well, and the inversion causes a failure to match because of the MSB.
    // It also works if you do the inversion separately ( ie: b = ~b; if (a != b) return 0; )
    // we can also use signed chars, which causes the promotions to sign extend and
    // accidentally give us the correct results.
    if (a != (unsigned char)(~b)) return 0;
    return 1;
}

// destroys the magic byte so high scores will be reset
void clearHighScores() {
    // unlock the eeprom
	GromWriteData(0xffff, 15, 0x55);
	GromWriteData(0xffff, 15, 0xaa);
	GromWriteData(0xffff, 15, 0x5a);

    // now write the data to corrupt the magic
    GromWriteData(UBERGROM_WRITE, 15, 0);

    // and relock the eeprom
	GromWriteData(0xffff, 15, 0);
}

void readHighScores(struct _scores *scores) {
    // we think the device exists, so suck in the high scores
    // we don't bother assume the cache is valid - we re-read each time
    unsigned char *pDat = (unsigned char*)scores;
    GROM_SET_ADDRESS(UBERGROM_WRITE);
    for (int idx=UBERGROM_WRITE; idx<UBERGROM_WRITE+sizeof(struct _scores); ++idx) {
        *(pDat++) = UBERGROM_RD;
    }

    // is it a valid high score table?
    if (scores->magic != UBERGROM_MAGIC) {
        // No. Load a default one
        scores->magic = UBERGROM_MAGIC;
        for (int idx=0; idx<10; ++idx) {
            scores->entry[idx].val = (10-idx)*100;
            memset(&scores->entry[idx].data, 'A', 4);
            scores->entry[idx].data[0]=0;
        }
        // make sure the flash is valid
        saveScores(scores);
    }
}

void displayHighScores(struct _scores *scores) {
    // get the border graphics
    vdpmemcpy(0x2000+160*8, LCARS, sizeof(LCARS));
    vdpchar(gColor+20, COLOR_LTRED<<4);

	cls();

    wrapinitstars();
	level = 1;	// to make sure we get stars

    // draw the frames
    vchar(0,7,165,24);
    vchar(0,25,165,24);
    hchar(0,11,161,11);
    hchar(2,11,166,11);
    xchar(0,10,160);
    xchar(2,10,163);
    xchar(0,22,162);
    xchar(2,22,164);
    xchar(1,10,165);
    xchar(1,22,165);

    // print the title
    writestring(1, 11, "HIGH SCORES");

    // emit the scores
    { 
        unsigned int oldscore, oldscoremode;
        oldscore = score;
        oldscoremode = scoremode;

        for (int idx=0; idx<10; ++idx) {
            score = scores->entry[idx].val;
            scoremode = scores->entry[idx].data[0];
	        VDP_SET_ADDRESS_WRITE((unsigned int)(gIMAGE+VDP_SCREEN_POS((idx*2)+4,11)));
            displayScore();
            VDPWD=' ';
            VDPWD=scores->entry[idx].data[1];
            VDPWD=scores->entry[idx].data[2];
            VDPWD=scores->entry[idx].data[3];
        }

        score = oldscore;
        scoremode = oldscoremode;
    }
}

// high scores are enabled only if an ubergrom is attached and
// configured. Since we don't need much space, we'll just
// use the configuration area at base 15, address >f800
// we won't worry about the config, we'll just use the
// eeprom starting at >fa00 (well beyond the max config space)
void showHighScores() {
    struct _scores scores;

    // get scores (will load default table if needed)
    readHighScores(&scores);

    // put on screen
    displayHighScores(&scores);

    // wait for exit
    unsigned char cnt;
    int frames = 1000;
	while (frames--) {
		cnt = VDP_INT_COUNTER;
		for (;;) {
			kscanfast(1);
            if (KSCAN_KEY == '0') return;
			if (KSCAN_KEY == JOY_FIRE) return;
			VDP_INT_POLL;
			if (cnt != VDP_INT_COUNTER) break;

			kscanfast(2);
			if (KSCAN_KEY == JOY_FIRE) return;
			VDP_INT_POLL;
			if (cnt != VDP_INT_COUNTER) break;
		}
        checkQuit();
        wrapbackground();		// for stars
	}
}

// enter the high score, if earned
// warning: may or may not return - we reboot here if a high score is entered
void registerHiScore() {
#define LONG_DELAY 45
#define SHORT_DELAY 5

	int scoreidx,i;
	int pos, chr;
	int delay=LONG_DELAY;
    struct _scores scores;

    // is the ubergrom present?
    if (!checkHighScores()) {
        return;
    }

    // no entry if cheating
    if (scoremode == 9) {
        return;
    }

    // get scores (will load default table if needed)
    readHighScores(&scores);

    // first determine if the player even earned a high score,
	// and if so, which one. If not, we can just return.
	for (scoreidx=0; scoreidx<10; scoreidx++) {
		if (score > scores.entry[scoreidx].val) {
			break;
		}
        if ((score == scores.entry[scoreidx].val) && (scoremode > scores.entry[scoreidx].data[0])) {
            break;
        }
	}
	if (scoreidx > 9) {
		// no high score
		return;
	}

	// shift down the lower scores
	for (i=9; i>scoreidx; i--) {
        scores.entry[i].val = scores.entry[i-1].val;
        scores.entry[i].data[0] = scores.entry[i-1].data[0];
        scores.entry[i].data[1] = scores.entry[i-1].data[1];
        scores.entry[i].data[2] = scores.entry[i-1].data[2];
        scores.entry[i].data[3] = scores.entry[i-1].data[3];
    }

	// set up the high score so it draws nicely
	scores.entry[scoreidx].val=score;
    scores.entry[scoreidx].data[0] = scoremode;
    scores.entry[scoreidx].data[1] =' ';
    scores.entry[scoreidx].data[2] =' ';
    scores.entry[scoreidx].data[3] =' ';

    // now get the high scores up on screen
    displayHighScores(&scores);

    int row = scoreidx*2+4;     // which row we are working on
    const int col = 19;

    // highlight the row
    vchar(0, 7, 32, row);
    vchar(0, 25, 32, row);
    xchar(row,  7, 160);
    xchar(row, 25, 162);
    hchar(row,  8, 161, 2);
    hchar(row, 23, 161, 2);

    pos = 0;    // which position (0-2) (note the array is 1,2,3 - 0 is the scoremode)
    chr = 0;    // which character (0-27 = A-Z,.,<<)

	for (;;) {
		waitforstep();

        checkQuit();
        wrapbackground();		// for stars

        joystfast(joynum);
        if ((KSCAN_JOYX == 0)&&(KSCAN_KEY == 0xff)) {
			delay=0;
		}

		if (delay>1) {
			delay--;
		} else {
			if (KSCAN_JOYX == JOY_LEFT) {
				chr--;
				if (chr < 0) chr=27;    // one past 'Z'
				if (delay == 1) {
					delay=SHORT_DELAY;
				} else {
					delay=LONG_DELAY;
				}
			}
			if (KSCAN_JOYX == JOY_RIGHT) {
				chr++;
                if (chr > 27) chr=0;    // back to 'A'
				if (delay == 1) {
					delay=SHORT_DELAY;
				} else {
					delay=LONG_DELAY;
				}
			}

            kscanfast(joynum);
		    if (KSCAN_KEY == JOY_FIRE) {
			    switch (chr) {
				    case 27:
					    // back up selected
					    if (pos > 0) {
                            xchar(row, col+pos, ' ');
                            --pos;
                            scores.entry[scoreidx].data[pos+1]=' ';
					    }
                        delay = LONG_DELAY;
					    break;

				    case 26:
					    // period selected
                        scores.entry[scoreidx].data[pos+1]='.';
                        xchar(row, col+pos, '.');
                        ++pos;
                        delay = LONG_DELAY;
					    break;

				    default:
					    // letter selected
                        scores.entry[scoreidx].data[pos+1]=chr+'A';
                        xchar(row, col+pos, chr+'A');
                        ++pos;
                        delay = LONG_DELAY;
					    break;
			    }
		    }
        }

        if (pos > 2) break;

        // update display
        switch (chr) {
            case 27:    xchar(row, col+pos, '<'); break;
            case 26:    xchar(row, col+pos, '.'); break;
            default:    xchar(row, col+pos, chr+'A'); break;
        }
	}

    // save the scores to the cart
    saveScores(&scores);

    // wait for key release
    while (KSCAN_KEY != 0xff) {
        kscanfast(joynum);
    }

    // and run high scores normally
    showHighScores();
}

// just a wrapper for showHighScores so we can reboot on exit
void doShowHighScores() {
	showHighScores();
	reboot();
}

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

// simple sound test
char * const musText[] = {
    "Press:",
    "  1-9 for MUSIC, 0 to STOP",
    "  A   to set MUSIC+SFX SID",
    "  B   to set MUSIC+SFX FORTI",
    "  C   to set MUSIC ONLY",
    "  D   to set SFX ONLY",
    "  F   ",
    "  H   to CLEAR HIGH SCORES"
};

const unsigned char VUMETER[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7e,
    0x00,0x00,0x00,0x00,0x00,0x00,0x7e,0x7e,
    0x00,0x00,0x00,0x00,0x00,0x7e,0x7e,0x7e,
    0x00,0x00,0x00,0x00,0x7e,0x7e,0x7e,0x7e,
    0x00,0x00,0x00,0x7e,0x7e,0x7e,0x7e,0x7e,
    0x00,0x00,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,
    0x00,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,
    0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e
};

const char SetStr[]  = "\x8  using";
const char UnsetStr[] =  "  to set";

// To be called only from getDifficulty()
void soundtest() {
	unsigned char r;

repaint:
	cls();

    // load vu graphics
    vdpmemcpy(gPATTERN+168*8, VUMETER, sizeof(VUMETER));
    vdpchar(gCOLOR+21, COLOR_LTYELLOW<<4);

    // draw boxes
    hchar(0, 6, 161, 26);
    hchar(16, 1, 166, 31);
    hchar(17, 7, 161, 18);
    hchar(19, 7, 166, 18);
    hchar(20, 0, 161, 32);
    vchar(1, 0, 165, 15);
    xchar(0, 0, 160);
    xchar(16, 0, 163);
    xchar(17, 6, 160);
    xchar(19, 6, 163);
    xchar(17, 25, 162);
    xchar(19, 25, 164);
    xchar(18, 6, 165);
    xchar(18, 25, 165);

    // draw menu
	for (r=0; r<8; ++r) {
		writestring(r<<1, 1, musText[r]);
	}
    writestring(18, 7, "Press FIRE to EXIT");

    if (doMusic != doMusicOnly) {
        // don't bother with the SFX hint if SFX are turned off
        writestring(21, 1, "(HINT: Hold UP and # for SFX)");
    }

	wrapinitstars();
	level = 1;	// to make sure we get stars

    // wait for key release
    while (KSCAN_KEY != 0xff) {
        kscanfast(0);
    }

	for (;;) {
        // update cursor
        writestring(4, 5, (char*)(doMusic==doAllMusic   ? SetStr:UnsetStr));
        writestring(6, 5, (char*)(doMusic==doForTI      ? SetStr:UnsetStr));
        writestring(8,5, (char*)(doMusic==doMusicOnly  ? SetStr:UnsetStr));
        writestring(10,5, (char*)(doMusic==doSfxInstead ? SetStr:UnsetStr));

        if (!realf18a) {
            writestring(12, 3, "    F18A not installed");
        } else if (f18a) {
            writestring(12, 7, "to restart WITHOUT F18A");
        } else {
            writestring(12, 7, "to restart WITH F18A");
        }

		waitforstep();

        // update VU Meters
        for (int idx=0; idx<4; ++idx) {
            unsigned int v=15-(songVol[idx]&0xf);
            unsigned int adr = gIMAGE+VDP_SCREEN_POS(19,3+idx);
            if (idx>1) adr+=22;
            if (v == 0) {
                vdpchar(adr, 32);
                vdpchar(adr-32, 32);
                continue;
            } else if (v == 8) {
                vdpchar(adr, 175);
                continue;
            } else if (v>8) {
                vdpchar(adr, 175);
                adr-=32;
                v-=8;
            } else {
                vdpchar(adr-32, 32);
            }
            vdpchar(adr, 168+v);
        }

        // handle input
		joystfast(joynum);
		kscanfast(joynum);	// all keys except '*' are okay (cause we are called with '*' down)

		if ((KSCAN_KEY != 0xff)&&(KSCAN_JOYY != JOY_UP)) {
			shutup();
		}

		switch (KSCAN_KEY) {
		case JOY_FIRE:	
			return;

		case '1':
			if (KSCAN_JOYY == JOY_UP) {
				playsfx_armor();
			} else {
				StartMusic(STAGE1MUS, 1);
			}
			break;

		case '2':
			if (KSCAN_JOYY == JOY_UP) {
				playsfx_explosion();
			} else {
				StartMusic(STAGE2MUS, 1);
			}
			break;

		case '3':
			if (KSCAN_JOYY == JOY_UP) {
				playsfx_hitboss();
			} else {
				StartMusic(STAGE3MUS, 1);
			}
			break;

		case '4':
			if (KSCAN_JOYY == JOY_UP) {
				playsfx_nukebomb();
			} else {
				StartMusic(STAGE4MUS, 1);
			}
			break;

		case '5':
			if (KSCAN_JOYY == JOY_UP) {
				playsfx_shipdead();
			} else {
				StartMusic(STAGE5MUS, 1);
			}
			break;

		case '6':
			if (KSCAN_JOYY == JOY_UP) {
				playsfx_shielddown();
			} else {
				StartMusic(BOSSMUS, 1);
			}
			break;

		case '7':
			if (KSCAN_JOYY == JOY_UP) {
				playsfx_shieldwarn();
			} else {
				StartMusic(GAMEOVERMUS, 0);
			}
			break;

		case '8':
			if (KSCAN_JOYY == JOY_UP) {
				playsfx_shieldup();
			} else {
				StartMusic(WINSCROLLMUS, 1);
			}
			break;

		case '9':
			if (KSCAN_JOYY == JOY_UP) {
				playsfx_pwrpulse();
			} else {
    			StartMusic(WINANIMMUS, 0);
			}
			break;

		case '0':
			if (KSCAN_JOYY == JOY_UP) {
				playsfx_pwrwide();
			} else {
                if (KSCAN_JOYY == JOY_DOWN) {
    				nDifficulty = DIFFICULTY_HARD;
	    			wrapGamWin();
                }
			}
            break;

        case 'A':
            doMusic = doAllMusic;
            writestring(21,0, "Normal setting for most systems.");
            writestring(23,0, "(It's okay if you have no SID.) ");
            //                 01234567890123456789012345678901
            *SAVEDMUSIC = (unsigned int)doMusic;
            wrapSaveScores(NULL);
            break;

        case 'B':
            doMusic = doForTI;
            writestring(21,0, " If you have a FORTI card avail ");
            writestring(23,0, "(Music and SFX will play on 1&2)");
            //                 01234567890123456789012345678901
            *SAVEDMUSIC = (unsigned int)doMusic;
            wrapSaveScores(NULL);
            break;

        case 'C':
            doMusic = doMusicOnly;
            writestring(21,0, "If your SID causes unwanted snd ");
            writestring(23,0, "(Some SID clones may sound poor)");
            //                 01234567890123456789012345678901
            *SAVEDMUSIC = (unsigned int)doMusic;
            wrapSaveScores(NULL);
            break;

        case 'D':
            doMusic = doSfxInstead;
            writestring(21,0, " Disable music, only play SFX.  ");
            writestring(23,0, " (If you don't like the music.) ");
            //                 01234567890123456789012345678901
            *SAVEDMUSIC = (unsigned int)doMusic;
            wrapSaveScores(NULL);
            break;

        case 'F':
            if (realf18a) {
                if (f18a) {
                    f18a = 0;
                } else {
                    f18a = 1;
                }
                reboot();
            }
            break;

        case 'H':
            for (int i=2; i<15; i+=2) {
                hchar(i, 3, 32, 29);
            }
            if (!wrapCheckHighScores()) {
                writestring(8, 6, "No high score memory");
            } else {
                writestring(8, 6, "Clear High Scores? Y/N");
                while ((KSCAN_KEY != 'Y') && (KSCAN_KEY != 'N')) {
                    kscanfast(0);
                }
                if (KSCAN_KEY == 'Y') {
                    wrapClearHighScores();
                    writestring(8, 6, "High scores cleared   ");
                } else {
                    // no need to wait in the N case
                    goto repaint;
                }
            }
            while (KSCAN_KEY != 0xff) {
                kscanfast(0);
            };
            writestring(10, 9, "Press any key");
            while (KSCAN_KEY == 0xff) {
                kscanfast(0);
            }
            goto repaint;
		}

		// just for fun
        // wait for key release
        if (KSCAN_KEY != 0xff) {
            do {
		        wrapbackground();		// for stars
                kscanfast(joynum);
                waitforstep();
	        } while (KSCAN_KEY != 0xff);
        } else {
            wrapbackground();
        }
    }
}


// libti99
#include <vdp.h>
#include <sound.h>
#include <kscan.h>
#include <TISNPlay.h>

// game
#include "game.h"
#include "music.h"

extern const unsigned char sfx_armor[];
extern const unsigned char sfx_explosion[];
extern const unsigned char sfx_hitboss[];
extern const unsigned char sfx_nukebomb[];
extern const unsigned char sfx_shipdead[];
extern const unsigned char sfx_shielddown[];
extern const unsigned char sfx_shieldwarn[];
extern const unsigned char sfx_pwrwide[];
extern const unsigned char sfx_shieldup[];
extern const unsigned char sfx_pwrpulse[];
extern const unsigned char sfx_armor_sid[];
extern const unsigned char sfx_explosion_sid[];
extern const unsigned char sfx_hitboss_sid[];
extern const unsigned char sfx_nukebomb_sid[];
extern const unsigned char sfx_shipdead_sid[];
extern const unsigned char sfx_shielddown_sid[];
extern const unsigned char sfx_shieldwarn_sid[];
extern const unsigned char sfx_pwrwide_sid[];
extern const unsigned char sfx_shieldup_sid[];
extern const unsigned char sfx_pwrpulse_sid[];

// although the music lives in various banks, the
// player code is in the fixed bank, so we can
// just swap in before calling the playback code,
// then swap back as per normal.

const unsigned char *pLoopMus;
unsigned int  loopBank;
unsigned int  loopIdx;
unsigned int  musBank;
const unsigned char *pSfx;
const unsigned char *pShoot;
unsigned char blockSfx;

// AY emulation
static unsigned char a1,a2,b1,b2;

// SID emulation
static unsigned int freq1,freq2;
static unsigned char oldv1, oldv2, oldv3;

// we can change this out for the sound effect only version
void (*doMusic)(void);

// process a single SID command
unsigned char *processSID(unsigned char *pShoot) {
    unsigned char reg = *(pShoot++);
    switch (reg) {
        case 0x00:  /*freq1*/
        case 0x0e:  /*freq2*/
        case 0x1c:  /*freq3: frequency - 2 bytes */ 
        {
            volatile unsigned char *adr = (volatile unsigned char*)SID_BASE_ADDRESS+reg;
            *(adr)=*(pShoot++); 
            *(adr+2)=*(pShoot++); 
        }
        break;

        // volumes we'll do separate since we need to remember the value
        case 0x0c:  /*vol1*/
        {
            unsigned char val = *(pShoot++);
            SIDBLASTER_SR1 = val;
            if (val > oldv1) {
                SIDBLASTER_CR1=SIDBLASTER_CR_PULSE;
                SIDBLASTER_CR1=SIDBLASTER_CR_PULSE|SIDBLASTER_CR_GATE;
            } else if (val == 0) {
                SIDBLASTER_CR1=SIDBLASTER_CR_PULSE;
            }
            oldv1=val;
        }
        break;

        case 0x1a:  /*vol2*/
        {
            unsigned char val = *(pShoot++);
            SIDBLASTER_SR2 = val;
            if (val > oldv2) {
                SIDBLASTER_CR2=SIDBLASTER_CR_PULSE;
                SIDBLASTER_CR2=SIDBLASTER_CR_PULSE|SIDBLASTER_CR_GATE;
            } else if (val == 0) {
                SIDBLASTER_CR2=SIDBLASTER_CR_PULSE;
            }
            oldv2=val;
        }
        break;

        case 0x28:  /*vol3*/
        {
            unsigned char val = *(pShoot++);
            SIDBLASTER_SR3 = val;
            if (val > oldv3) {
                SIDBLASTER_CR3=SIDBLASTER_CR_PULSE;
                SIDBLASTER_CR3=SIDBLASTER_CR_PULSE|SIDBLASTER_CR_GATE;
            } else if (val == 0) {
                SIDBLASTER_CR3=SIDBLASTER_CR_PULSE;
            }
            oldv3=val;
        }
        break;
    }

    return pShoot;
}


// one interrupt of music (not called on interrupt)
void doAllMusic() {
	unsigned int old = nBank;

	// no music in demo, but sfx are okay
	if (joynum != 0) {
	    // check whether we're playing
	    SWITCH_IN_PREV_BANK(musBank);

        if (!(isSNPlaying)) {
		    // loop music if needed
		    if (pLoopMus != NULL) {
			    MUTE_SOUND();	// don't carry over any old tones
			    StartMusic(pLoopMus, loopBank, loopIdx, 1);
			    // we'll try not returning for smoother transition to intros,
			    // but, this player is still kind of heavy-weight.
		    } else {
                // firing this every frame seems excessive... but I guess
                // it's cheaper than playing the music
			    MUTE_SOUND();
			    goto checksfx;
		    }
	    }

        CALL_PLAYER_SN;
    }
     
checksfx:

    // run sound effects at 30 hz
    SWITCH_IN_BANK4a;

    if (VDP_INT_COUNTER & 1) {
        if (NULL != pSfx) {
            // SFX data format:
            // number registers, [register number, register data]
            unsigned char regs = *(pSfx++);
            if (0 == regs) {
                pSfx = NULL;
                // we'll undo the block next frame
            } else {
                while (regs--) {
                    pSfx = processSID(pSfx);
                }
            }
        } else {
            blockSfx = 0;   // make sure we didn't forget to clear something
        }
    } else {
        if (NULL != pShoot) {
            // SFX data format:
            // number registers, [register number, register data]
            unsigned char regs = *(pShoot++);
            if (0 == regs) {
                pShoot = NULL;
            } else {
                while (regs--) {
                    pShoot = processSID(pShoot);
                }
            }
        }
    }

	SWITCH_IN_PREV_BANK(old);
}

// does the AY to SN conversion
// we assume SN compatible data, since there's nothing else today
// and we assume there's never a high tone byte not followed by a low
const unsigned char noisemap[8] = { 0xe4,0xe4,0xe5,0xe5,0xe6,0xe6,0xe6,0xe6 };
void wrapAYcmd(unsigned char reg, unsigned char dat) {
    switch (reg) {
        case 0: /* al */ a1=0x80|(dat&0x0f); a2=(a2&0xf0)|((dat&0xf0)>>4); SOUND=a1; SOUND=a2; break;
        case 1: /* ah */ a2=(dat<<4)|(a2&0x0f); break;  // assume a low is coming
        case 2: /* bl */ b1=0xa0|(dat&0x0f); b2=(b2&0xf0)|((dat&0xf0)>>4); SOUND=b1; SOUND=b2; break;
        case 3: /* bh */ b2=(dat<<4)|(b2&0x0f); break;  // assume a low is coming
        case 6: /* noi */ SOUND=noisemap[dat>>5]; break;
        case 8: /* vola */ SOUND=(15-dat)|0x90; break;
        case 9: /* volb */ SOUND=(15-dat)|0xb0; break;
        case 10: /* vola */ SOUND=(15-dat)|0xf0; break;
    }
}

#if 0
// this works=ish, but the resolution is too low for good sound and noises didn't work
// the correct math is 1876713/cnt = sidcnt - but can't do that in 16 bits
// do AY to SID conversion
// same idea as wrapAYcmd, but target the SID
// We don't need to bank the SID in cause we never bank it out after startup
const unsigned char noisemapsid[8] = { 0xF0, 0xd0, 0xb0, 0x90, 0x70, 0x50, 0x30, 0x10 };
void wrapAY2SID(unsigned char reg, unsigned char dat) {
    switch (reg) {
        case 0: /* al */ freq1=freq1|dat; freq1=(unsigned)58647/(freq1>>5); SIDBLASTER_FREQHI1=freq1>>8; SIDBLASTER_FREQLO1=freq1&0xff; break;
        case 1: /* ah */ freq1=dat<<8; break;     // assume a low is coming
        case 2: /* bl */ freq2=freq2|dat; freq2=(unsigned)58647/(freq2>>5); SIDBLASTER_FREQHI2=freq2>>8; SIDBLASTER_FREQLO2=freq2&0xff; break;
        case 3: /* bh */ freq2=dat<<8; break;     // assume a low is coming
        case 6: /* noi */ SIDBLASTER_FREQHI3=noisemap[dat>>5]; SIDBLASTER_FREQLO3=0; break;
        case 8: /* vola */ SIDBLASTER_SR1=(dat&0xf)<<4; if (dat>oldv1) { SIDBLASTER_CR1=SIDBLASTER_CR_PULSE; SIDBLASTER_CR1=SIDBLASTER_CR_PULSE|SIDBLASTER_CR_GATE; } oldv1=dat; break;
        case 9: /* volb */ SIDBLASTER_SR2=(dat&0xf)<<4; if (dat>oldv2) { SIDBLASTER_CR2=SIDBLASTER_CR_PULSE; SIDBLASTER_CR2=SIDBLASTER_CR_PULSE|SIDBLASTER_CR_GATE; } oldv2=dat; break;
        case 10: /* volc */ SIDBLASTER_SR3=(dat&0xf)<<4; if (dat>oldv3) { SIDBLASTER_CR3=SIDBLASTER_CR_PULSE; SIDBLASTER_CR3=SIDBLASTER_CR_PULSE|SIDBLASTER_CR_GATE; } oldv3=dat; break;
    }
}
#endif

// instead of music, do just SFX and convert it for the SN chip
// no bank switch needed, but we do need to convert the AY data,
// which I will do in real time since it's still quicker than 
// the music player was
void doSfxInstead() {
    // if any music is active, stop it
    if (isSNPlaying) {
        StopSong();
        // don't shutup and don't kill loop music, in case it gets turned back on
    }

	unsigned int old = nBank;
    SWITCH_IN_BANK4a;

    // run sound effects at 30 hz
    if (VDP_INT_COUNTER & 1) {
        if (NULL != pSfx) {
            // SFX data format:
            // number registers, [register number, register data]
            unsigned char regs = *(pSfx++);
            if (0 == regs) {
                pSfx = NULL;
                blockSfx = 0;
            } else {
                while (regs--) {
                    unsigned char reg = *(pSfx++);
                    wrapAYcmd(reg, *(pSfx++));
                }
            }
        }
    } else {
        if (NULL != pShoot) {
            // SFX data format:
            // number registers, [register number, register data]
            unsigned char regs = *(pShoot++);
            if (0 == regs) {
                pShoot = NULL;
            } else {
                while (regs--) {
                    unsigned char reg = *(pShoot++);
                    wrapAYcmd(reg, *(pShoot++));
                }
            }
        }
    }

    SWITCH_IN_PREV_BANK(old);
}

void StartMusic(const unsigned char *p, unsigned int inBank, unsigned int idx, unsigned int bLoop) {
	unsigned int old = nBank;

	// no music in demo
	if (joynum == 0) {
		shutup();
		pLoopMus = NULL;
		return;
	}

	musBank = inBank; 
	if (bLoop) {
		pLoopMus = p;
		loopBank = inBank;
		loopIdx  = idx;
	} else { 
		pLoopMus = NULL;
	}
	SWITCH_IN_PREV_BANK(musBank);
	//stinit((unsigned char*)p, idx);
    StartSong(p, idx);
	SWITCH_IN_PREV_BANK(old);
}

void shutup()
{ 
	/*silence to music generators */
	//allstop();
    StopSong();
	MUTE_SOUND();
	pLoopMus=NULL;

    pSfx = NULL;
    pShoot = NULL;

    // just turn off all the SID gates
    SIDBLASTER_CR1 = SIDBLASTER_CR_PULSE;
    SIDBLASTER_CR2 = SIDBLASTER_CR_PULSE;
    SIDBLASTER_CR3 = SIDBLASTER_CR_NOISE;
}

// do any necessary sound chip initialization
void initSound() {
    shutup();
    //doMusic = doAllMusic;   // this is how we do music mute   (this is initialized early now as it's saved on the stack)
    a1=0;
    a2=0;
    b1=0;
    b2=0;

    // set up the SID so 1 and 2 are tone channels, and 3 is noise
    SIDBLASTER_CR1 = SIDBLASTER_CR_PULSE;
    SIDBLASTER_CR2 = SIDBLASTER_CR_PULSE;
    SIDBLASTER_CR3 = SIDBLASTER_CR_NOISE;
    SIDBLASTER_PWHI1 = 0x08;
    SIDBLASTER_PWHI2 = 0x08;
    SIDBLASTER_PWLO1 = 0;
    SIDBLASTER_PWLO2 = 0;
    SIDBLASTER_AD1 = 0;
    SIDBLASTER_AD2 = 0;
    SIDBLASTER_AD3 = 0;
    SIDBLASTER_SR1 = 0;
    SIDBLASTER_SR2 = 0;
    SIDBLASTER_SR3 = 0;
    SIDBLASTER_MODEVOL = 0xf;   // maximum volume

    freq1=0;
    freq2=0;
    oldv1=0;
    oldv2=0;
    oldv3=0;
}

// hit an armored enemy
void playsfx_armor() {
    if (blockSfx < 2) {
        if (doMusic != doSfxInstead) {
            pSfx = sfx_armor_sid;
        } else {
            pSfx = sfx_armor;
        }
        blockSfx = 1;
    }
}
// boss engine explodes
void playsfx_explosion() {
    if (doMusic != doSfxInstead) {
        pSfx = sfx_explosion_sid;
    } else {
        pSfx = sfx_explosion;
    }
    blockSfx = 2;
}
// hit boss body
void playsfx_hitboss() {
    if (!blockSfx) {
        if (doMusic != doSfxInstead) {
            pSfx = sfx_hitboss_sid;
        } else {
            pSfx = sfx_hitboss;
        }
    }
}
// blow up a nuke
void playsfx_nukebomb() {
    if (doMusic != doSfxInstead) {
        pSfx = sfx_nukebomb_sid;
    } else {
        pSfx = sfx_nukebomb;
    }
    blockSfx = 2;
}
// enemy ship dead
void playsfx_shipdead() {
    if (!blockSfx) {
        if (doMusic != doSfxInstead) {
            pSfx = sfx_shipdead_sid;
        } else {
            pSfx = sfx_shipdead;
        }
    }
}

// player sfxs run separately and sparingly

// shield offline
void playsfx_shielddown() {
    if (doMusic != doSfxInstead) {
        pShoot = sfx_shielddown_sid;
    } else {
        pShoot = sfx_shielddown;
    }
}
// shield powerup
void playsfx_shieldup() {
    if (doMusic != doSfxInstead) {
        pShoot = sfx_shieldup_sid;
    } else {
        pShoot = sfx_shieldup;
    }
}
// shield about to expire
void playsfx_shieldwarn() {
    if (doMusic != doSfxInstead) {
        pShoot = sfx_shieldwarn_sid;
    } else {
        pShoot = sfx_shieldwarn;
    }
}
// pulse weapon powerup
void playsfx_pwrpulse() {
    if (doMusic != doSfxInstead) {
        pShoot = sfx_pwrpulse_sid;
    } else {
        pShoot = sfx_pwrpulse;
    }
}
// wide shot powerup
void playsfx_pwrwide() {
    if (doMusic != doSfxInstead) {
        pShoot = sfx_pwrwide_sid;
    } else {
        pShoot = sfx_pwrwide;
    }
}


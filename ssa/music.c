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
const unsigned char *processSID(const unsigned char *pShoot) {
    unsigned char reg = *(pShoot++);
    switch (reg) {
        case 0x00:  /*freq1*/
        case 0x0e:  /*freq2*/
        case 0x1c:  /*freq3: noise - 2 bytes */ 
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
                SIDBLASTER_CR3=SIDBLASTER_CR_NOISE;
                SIDBLASTER_CR3=SIDBLASTER_CR_NOISE|SIDBLASTER_CR_GATE;
            } else if (val == 0) {
                SIDBLASTER_CR3=SIDBLASTER_CR_NOISE;
            }
            oldv3=val;
        }
        break;
    }

    return pShoot;
}

// run the music on the specified soundchip (used for stock and ForTI)
void playSNMusic(unsigned int soundChipAddress) {
	// check whether we're playing
	SWITCH_IN_PREV_BANK(musBank);

    if (!(isSNPlaying)) {
		// loop music if needed
        // - we mute only the passed in sound chip! otherwise we'll kill sfx on the forti card
        volatile unsigned char *aud = (unsigned char*)soundChipAddress;
        // firing this every frame seems excessive... but I guess
        // it's cheaper than playing the music
        *aud=TONE1_VOL|0x0f; 
        *aud=TONE2_VOL|0x0f; 
        *aud=TONE3_VOL|0x0f; 
        *aud=NOISE_VOL|0x0f;
		if (pLoopMus != NULL) {
			StartMusic(pLoopMus, loopBank, loopIdx, 1);
			// we'll try not returning for smoother transition to intros,
			// but, this player is still kind of heavy-weight.
		} else {
			return;
		}
	}

    // replacement for CALL_PLAYER_SN that wraps it and sets the sound chip address as requested
    //CALL_PLAYER_SN;
    // This is hacky -- because only r8 is not declared (r10 is stack) the compiler will put our data in r8 where we need it,
    // but it's not a guaranteed fix
    __asm__ volatile (                                              \
        "bl @Song2Lp"                                               \
        : /* no outputs */                                          \
        : [adr]"r"(soundChipAddress)                                \
        : "r0","r1","r2","r3","r4","r5","r6","r7","r9","r11","r12","r13","r14","r15","cc"   \
        );
}

// one interrupt of music (not called on interrupt)

// SN music, no sound effects (for cases where the SID is a poor quality clone)
void doMusicOnly() {
	unsigned int old = nBank;

	// no music in demo, but sfx are okay
	if (joynum != 0) {
        // warning: changes bank
        playSNMusic(SOUNDCHIP);    // stock sound chip
    }

	SWITCH_IN_PREV_BANK(old);
}

// regular case - music plus sound effects on SID (if present)
void doAllMusic() {
	unsigned int old = nBank;

	// no music in demo, but sfx are okay
	if (joynum != 0) {
        // warning: changes bank
        playSNMusic(SOUNDCHIP);    // stock sound chip
    }
     
    // run sound effects at 30 hz - SID version
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
void wrapAYcmd(volatile unsigned char *soundchip, unsigned char reg, unsigned char dat) {
    switch (reg) {
        case 0: /* al */ a1=0x80|(dat&0x0f); a2=(a2&0xf0)|((dat&0xf0)>>4); *soundchip=a1; *soundchip=a2; break;
        case 1: /* ah */ a2=(dat<<4)|(a2&0x0f); break;  // assume a low is coming
        case 2: /* bl */ b1=0xa0|(dat&0x0f); b2=(b2&0xf0)|((dat&0xf0)>>4); *soundchip=b1; *soundchip=b2; break;
        case 3: /* bh */ b2=(dat<<4)|(b2&0x0f); break;  // assume a low is coming
        case 6: /* noi */ *soundchip=noisemap[dat>>5]; break;
        case 8: /* vola */ *soundchip=(15-dat)|0x90; break;
        case 9: /* volb */ *soundchip=(15-dat)|0xb0; break;
        case 10: /* volc */ *soundchip=(15-dat)|0xf0; break;
    }
}

// instead of music, do just SFX and convert it for the SN chip
// no bank switch needed, but we do need to convert the AY data,
// which I will do in real time since it's still quicker than 
// the music player was.
void doSnSfx(unsigned char *soundchip) {
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
                    wrapAYcmd(soundchip, reg, *(pSfx++));
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
                    wrapAYcmd(soundchip, reg, *(pShoot++));
                }
            }
        }
    }

    SWITCH_IN_PREV_BANK(old);
}

// Sound effects on the stock SN, no music
void doSfxInstead() {
    doSnSfx((unsigned char*)SOUNDCHIP);    // stock sound chip
}

// ForTI card version - we only use chips 0 and 1
void doForTI() {
	unsigned int old = nBank;

    // no music in demo, but sfx are okay
	if (joynum != 0) {
        // warning: changes bank
        playSNMusic(FORTI_CHIP1);    // chip 1
    }

    doSnSfx((unsigned char*)FORTI_CHIP2);    // chip 2

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
    blockSfx = 0;

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
        if (doMusic == doAllMusic) {
            pSfx = sfx_armor_sid;
        } else {
            pSfx = sfx_armor;
        }
        blockSfx = 1;
    }
}
// boss engine explodes
void playsfx_explosion() {
    if (doMusic == doAllMusic) {
        pSfx = sfx_explosion_sid;
    } else {
        pSfx = sfx_explosion;
    }
    blockSfx = 2;
}
// hit boss body
void playsfx_hitboss() {
    if (!blockSfx) {
        if (doMusic == doAllMusic) {
            pSfx = sfx_hitboss_sid;
        } else {
            pSfx = sfx_hitboss;
        }
    }
}
// blow up a nuke
void playsfx_nukebomb() {
    if (doMusic == doAllMusic) {
        pSfx = sfx_nukebomb_sid;
    } else {
        pSfx = sfx_nukebomb;
    }
    blockSfx = 2;
}
// enemy ship dead
void playsfx_shipdead() {
    if (!blockSfx) {
        if (doMusic == doAllMusic) {
            pSfx = sfx_shipdead_sid;
        } else {
            pSfx = sfx_shipdead;
        }
    }
}

// player sfxs run separately and sparingly

// shield offline
void playsfx_shielddown() {
    if (doMusic == doAllMusic) {
        pShoot = sfx_shielddown_sid;
    } else {
        pShoot = sfx_shielddown;
    }
}
// shield powerup
void playsfx_shieldup() {
    if (doMusic == doAllMusic) {
        pShoot = sfx_shieldup_sid;
    } else {
        pShoot = sfx_shieldup;
    }
}
// shield about to expire
void playsfx_shieldwarn() {
    if (doMusic == doAllMusic) {
        pShoot = sfx_shieldwarn_sid;
    } else {
        pShoot = sfx_shieldwarn;
    }
}
// pulse weapon powerup
void playsfx_pwrpulse() {
    if (doMusic == doAllMusic) {
        pShoot = sfx_pwrpulse_sid;
    } else {
        pShoot = sfx_pwrpulse;
    }
}
// wide shot powerup
void playsfx_pwrwide() {
    if (doMusic == doAllMusic) {
        pShoot = sfx_pwrwide_sid;
    } else {
        pShoot = sfx_pwrwide;
    }
}


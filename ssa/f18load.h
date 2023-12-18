void initF18GPU();
void initCruiserf18();
void initSnowballf18();
void initLadybugf18();
void initGnatf18();
void initSelenaf18();

// write address to damage to 0x3c80 and anything to 3c82 (so, just write 3 VDP bytes)
#define GPU_DAMAGEIN 0x3c80
#define GPU_DAMAGEGO 0x3c82

#if 0
// palette indexes (4 color sprite mode)
#define PAL_SAUCER      4
#define PAL_JET         5
#define PAL_MINE        1
#define PAL_COPTER      6
#define PAL_SWIRL       7
#define PAL_BOMB        7
#define PAL_EXPLODE     8
#define PAL_BOSSFLAME   8
#define PAL_PULSE       10
#define PAL_SHIPFLAME   8
#define PAL_BEAM        2
#define PAL_MINETIP     2
#define PAL_SPREAD      11
#define PAL_PLAYHOME    10
#define PAL_BEAMGEN     4
#define PAL_PLAYSHIELD  13
#define PAL_PLAYSHIP    12
#define PAL_BOSSHOMETIP 14
#define PAL_BOSSHOMELT  3
#define PAL_BOSSHOMEDK  15
#define PAL_INVISIBLE   4
#define PAL_SHOT        9
#define PAL_BOSS        4       // boss uses 4 palettes
#elsif 0
// palette indexes (8 color sprite mode)
// Still need to define the palettes as if they were 4 color, but the least significant bit is ignored
#define PAL_SAUCER      4
#define PAL_JET         4
#define PAL_MINE        0
#define PAL_COPTER      6
#define PAL_SWIRL       6
#define PAL_BOMB        6
#define PAL_EXPLODE     8
#define PAL_BOSSFLAME   8
#define PAL_PULSE       10
#define PAL_SHIPFLAME   8
#define PAL_BEAM        2
#define PAL_MINETIP     2
#define PAL_SPREAD      10
#define PAL_PLAYHOME    10
#define PAL_BEAMGEN     4
#define PAL_BOSSHOMETIP 2
#define PAL_BOSSHOMELT  14
#define PAL_BOSSHOMEDK  14
#define PAL_INVISIBLE   4
#define PAL_SHOT        8
#define PAL_BOSS        4       // boss uses 2 8-color palettes, 4 4-color

#define PAL_PLAYSHIELD  14
#define PAL_PLAYSHIP    12
#else
// SL rendered 8 color palettes
// Still need to define the palettes as if they were 4 color, but the least significant bit is ignored
#define PAL_SAUCER      4
#define PAL_JET         6
#define PAL_MINE        10
#define PAL_COPTER      6
#define PAL_SWIRL       10
#define PAL_BOMB        12
#define PAL_EXPLODE     12
#define PAL_BOSSFLAME   12
#define PAL_PULSE       8
#define PAL_SHIPFLAME   12
#define PAL_BEAM        8
#define PAL_MINETIP     12
#define PAL_SPREAD      8
#define PAL_PLAYHOME    8
#define PAL_BEAMGEN     4
#define PAL_BOSSHOMETIP 2
#define PAL_BOSSHOMELT  0
#define PAL_BOSSHOMEDK  4
#define PAL_SHOT        8

// PAL_BOSS and PAL_INVISIBLE need to be the same
#define PAL_INVISIBLE   4
#define PAL_BOSS        4       // boss uses 2 8-color palettes, 4 4-color

#define PAL_PLAYSHIELD  0       // we steal a color from the normal TI palette
#define PAL_PLAYSHIP    14
#endif



void initF18GPU();
void initCruiserf18();
void initSnowballf18();
void initLadybugf18();
void initGnatf18();
void initSelenaf18();

// write address to damage to 0x3c80 and anything to 3c82 (so, just write 3 VDP bytes)
#define GPU_DAMAGEIN 0x3c80
#define GPU_DAMAGEGO 0x3c82

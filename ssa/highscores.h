#define UBERGROM_RD *((volatile unsigned char*)0x983c)
#define UBERGROM_WD *((volatile unsigned char*)0x9C3c)
#define UBERGROM_CHECK 0xf800
#define UBERGROM_WRITE 0xfa00
#define UBERGROM_UNLOCK 0xffff
#define UBERGROM_MAGIC 0x994A

// the high score table
struct _score {
    unsigned int val;
    unsigned char data[4];  // scoremode as a byte, then 3 initials
};
struct _scores {
    unsigned int magic;
    struct _score entry[10];
};

void saveScores(struct _scores *scores);
int checkHighScores();
void displayHighScores(struct _scores *scores);
void showHighScores();
void doShowHighScores();

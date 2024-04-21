/* program SUPER SPACE ACER design version 2.2 */
/* ported to ColecoVision by M.Brent */

// libti99
#include <vdp.h>
#include <kscan.h>
#include <system.h>
#include <grom.h>



void cls()
{ /* 'clear' screen with 32 */
	vdpmemset(gImage, 32, 40*24);
}

void history() {
	printf("This is a small archive of the original\n");
	printf("Super Space Acer.\n");
	printf("\n");
	printf("I started this game in 1988, while I\n");
	printf("still in High School. I had just bought\n");
	printf("some games from MicroPendium which had\n");
	printf("promised to push the TI to its limits,\n");
	printf("and I was quite disappointed.\n");
	printf("\n");
	printf("While I started it in assembly, I\n");
	printf("learned C not long after, and started\n");
	printf("to use C99 instead. Limits and my own\n");
	printf("inexperience cut the initial design back");
	printf("a bit, but I released it in 1992. I got\n");
	printf("a review in Micropendium, 2.5 stars,\n");
	printf("where the game was dinged for no score\n");
	printf("and no two player mode, despite praise\n");
	printf("for graphics, sound, responsiveness...\n");
	printf("I sold one copy.\n");
	printf("\n");
	printf("Well, I hope this version raises that\n");
	printf("to at least 3 stars, I added score!\n");
	printf("\n");
	printf("Press any key to return\n");

	KSCAN_KEY = 0xff;
	while (KSCAN_KEY == 0xff) {
		VDP_INT_POLL;
		kscanfast();
	}
}

void story() {

}

void game1() {

}

void game2() {

}

void main() {
	set_text();
	charsetlc();
	VDP_SET_REGISTER(VDP_REG_COL, (COLOR_GRAY<<4)|COLOR_DKBLUE);

//		    1234567890123456789012345678901234567890

	for (;;) {
		cls();
		printf("-- SUPER SPACE ACER --\n");
		printf("\n");
		printf("PRESS:\n");
		printf("  1 for History\n");
		printf("  2 for Story\n");
		printf("  3 for 1992 game\n");
		printf("  4 for unfinished sequel title page\n");
		printf("\n");
		printf("Or press QUIT to exit");

		KSCAN_KEY = 0xff;
		while ((KSCAN_KEY < '1') || (KSCAN_KEY > '4')) {
			VDP_INT_POLL;
			kscanfast();
		}

		switch (KSCAN_KEY) {
			case '1':
				history();
				break;

			case '2':
				story();
				break;

			case '3':
				game1();
				break;

			case '4':
				game2();
				break;
		}
	}
}
 

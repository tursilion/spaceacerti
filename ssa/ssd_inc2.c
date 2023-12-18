
// this seems fast enough that we don't have to pre-process the characters
// Not sure these two text blocks will line up, but GCC has a 1k limit
// on text strings, else it crashes with "stack smashing detected"
const char TXTDAT[] =
 " ============================== "
 "        Congratulations!        "
 "       You have defeated        "
 "      the evil Qwertians!       "
 "      The Galaxy is safe!       "
 "      You are a TRUE hero!      "
 " ============================== "
 "              IDEA              "
 "              ====              "
 "       Mike Brent (Tursi)       "
 "        Gordon Haddrell         "
 " ============================== "
 "            PROGRAM             "
 "            =======             "
 "      Mike Brent (again!)       "
 "      Released for TI-99        "
 "            in 1992             "
 "     Ported to ColecoVision     "
 "            for 2023            "
 " ============================== "
 "            GRAPHICS            "
 "            ========            "
 "      Mike Brent (still!)       "
 "         Brendan Wiese          "
 "      Light262 (DeviantArt)     "
 "     Some art assisted by AI    "
 " ============================== "
 "             MUSIC              "
 "             =====              "
 "     Tadd Nuznov (RushJet1)     "
 " ============================== ";
// space is character 0 in my font anyway, so deleting the
// first space in this one helps the text all line up
const char TXTDAT2[] = 
 "           PROPHET             "
 "            =======             "
 "          Greg jelmer           "     /* lowercase 'j' has the 'Z' character */
 " ============================== "
 "          EXTRA THANKS          "
 "          ============          "  
 "         Gord (again!!)         "
 "          Steve Brent           "
 "        Justin Goddard          "
 "          Jason Jones           "
 "         Chris Jonansen         "
 "          Steve Burns           "
 "        Lloyd Galenzoski        "
 "             Yurkie             "
 "             RasmusM            "
 "             TheMole            "
 "          Sometimes99er         "
 "          AtariAge.com          "
 "      TI Source - Texaments     "
 "           Dial-A-TI            "
 "             WEBBS              "
 "                                "
 "#"
 "                                "
 " ============================== "
 "                                "
 "        SUPER SPACE ACER        "
 "         by Mike Brent          "
 "   ` 1992 by Julius Software    "
 "                                "
 "       Final score              "
 "                                "
 "      now Harmlesslion.com      "
 "       ` 2023 Mike Brent        "
 "                                "
 " ============================== "
 "@";

// these are displayed at the end of the scrolltext
// but before the final screen on medium and hard.
// One is optionally selected each time.
// each line is exactly 32 characters for easy selection
// 8 tips - #0 is only medium, #7 is only hard!
// Do NOT rearrange these tips - they are checked for
// in winmed() so we don't display a tip the player
// is currently using or not eligable for.
// when adding tips, update the end of winmed.c to skip
// cheats the player is currently using
const char WINTIPS[] =
	 " Finish on hard for full ending "
	 " Try FIRE on ship select screen "
	 " Try DOWN for starting Snowball "
	 "    Secret ship GNAT is tiny    "
	 " Secret ship SELENE is powerful "
	 "LADYBIRD shields recharge on hit"
	 "Use the force  dont fire or miss"
	 " Hold UP to activate cheat mode "
	 "Isn't it fun to read through the binary?"
;


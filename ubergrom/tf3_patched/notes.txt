Wait for vblank" code checks for QUIT Fctn-=

Idea is to first flash the Sega logo on a black screen (animated!). Would be nice to add the text "NOT LICENSED OR ENDORSED BY SEGA"
	- if player presses FCTN-=, then exit (try to check this everywhere)

Then the title page (Playing title music)
	Three maps are provided, one for each third of the screen. This lets us preload all the needed characters.
	In each map, the top third is the background, the second third is the player's ship (2 and 3 only), and the third third is the title (1 and 2 only)
	
	First, load the background
	After a moment, flicker the ship in. To save characters crossing boundaries, flicker in directly left and right instead of diagonally. The ship only has 4 rows in the middle segment (and it's just 2 characters wide - the tailfin)
	Next, the title is supposed to slide in, this is all of segment 1 and 3 rows of segment 2. Don't MOVE parts (Genesis did that with sprites anyway), but draw the characters in alternating vertical stripes four characters wide - only pattern table updates so the border crossing is not a concern.
	Finally, the 'III' is supposed to dither in, it's just 6 by 4 characters, but to make it simple (since we don't know how magellen mapped the colors) we'll just dither in a character at time.
	- if player presses FCTN-=, then exit (try to check this everywhere)
	
Next, load the briefing map data. It's just one character set in all three banks, because I didn't plan it properly, but we're going to fake some of it. Note that
there is also 4 sprites - they are meant to be used as mag 4.
	- first is a square white box (15) for the weapon select
	- second is a vertical red line (7) to be repeated for the vertical stripe on the map - note, inset by 8 pixels, don't touch the walls. There will be 4.
	- third is a horizontal red line (7) to be repeated for the horizontal stripe on the map - again, inset by 8 pixels. There will be 3.
	- finally, a targeting pointer for the boss face. This will flicker when the stripe is low, but it's all we can do for now. Only 2 stages are an issue.
	
Before displaying the briefing map, however, extract just the map image and display it centered horizontally instead of in it's normal place.
	- display the text "Stage Select" at the top, and stage name at the bottom (playing stage select music)
	- start on Hydra, order is Hydra, Gorgon, Seiren, Haides, Ellis, Cerberus, ORN Base, ORN Core
	- if the user selects left and right, snap the crosshairs to the new target. Allow the last three stages
		- crosshair code will need a horizontal offset, it ALSO needs to work in the normal position
	- if player presses FCTN-=, then exit (try to check this everywhere)
	- on Space or Enter, advance and remember the choice, otherwise, wait till the song ends and assume hydra
	
crosshairs start at 0,0 offset, and move at a scaled rate (use 8.8 fixed point) to reach the target in about 3 seconds while music plays
	- the boss silhouette for each area is displayed on song change (remember to erase the old one)
	- the text for the area is displayed one line at a time using a cursor for effect - note different colors
	- at bottom, display string as below or something...
	- if player presses Comma or Period, move the weapon select (there are only two, so doesn't matter which)
	- if player presses 1-4, change the speed counters
	- if player presses 'N', go to NEXT song (restart program if no more, back through logo and title)
	- if player presses 'P', go to PREVIOUS song (repeat last if no more)
	- if player pressed Space, pause or unpause, and update text at the bottom appropriately
	- if player presses FCTN-=, then exit (try to check this everywhere)
	- if player presses 7, assume it was alt-7 and cycle text through help texts:
		- "song title"
		- MUSIC PLAYER BY TURSI
		- TF3 BY TECHNOSOFT
		- N FOR NEXT SONG
		- P FOR PREVIOUS SONG (previous song can go back to title, but does not change the screen!!)
		- SPACE TO PAUSE
	- when song ends, advance to the next song (or restart if finished) (DO NOT ADVANCE IF TEXT IS STILL DRAWING)
	- when Cerberus is reached, erase the sun and replace with the orn base map - it should fit entirely in bank 2 so can replace chars
		- no need to redraw the sun if go back or loops
	
Only a couple of boss silhouettes have one row in block 2, we'll manually split out the character maps.
	- remember to erase the grid with character 9 before changing patterns
	- free characters in block 2: we can always use chars 240-255, and that's enough for a full row.
	- so, copy the first 16 characters to block 2, and remap the first row only to start at 240 - it will always fit
Block 3 needs more. The larges silhouette needs 44 characters (ignoring how many are in block 2), but we never need yellow text or the top of the image:
	- so we can copy in starting at character 77, and 44 characters ARE free at that point. We can safely ignore block 2 issues and map blind.
	- remember to allow for interrupts - drawing one row at a time then waiting for vblank might not hurt, even if it draws slower

Song Order with text (text only shows for title and stage select if you go Previous). Bosses use the same text as their stage, except song title.
Display "Duration" instead of "Weak point"

	Title
		- The Wind Blew All Day Long
		- Stage: TITLE
		- Target: TF3
		- Duration: 0M15
		
	Stage Select
		- Beyond the Peace
		- Stage: STAGE SELECT
		- Target: CHOOSE
		- Duration: 1M01
		
	Hydra
		- Back to the Fire
		- Stage: HYDRA
		- Target: GARGOYLE
		- Duration: 2M15
	
	Hydra Boss
		- Gargoyle
		- Duration: 1M38
		
	Gorgon
		- Venus Fire
		- Stage: GORGON
		- Target: TWIN VULCAN
		- Duration: 2M54
		
	Gorgon Boss
		- Twin Vulcan
		- Duration: 1M11
		
	Seiren
		- The Grubby Dark Blue
		- Stage: SEIREN
		- Target: KING FISH
		- Duration: 2M32
		
	Seiren Boss
		- King Fish
		- Duration: 1M30
		
	Haides
		- Truth
		- Stage: HAIDES
		- Target: G LOBSTER
		- Duration: 1M43
		
	Haides Boss
		- G LOBSTER
		- Duration: 1M39
		
	Ellis
		- FINAL TAKE A CHANCE
		- Stage: ELLIS
		- Target: MOBILE FORT
		- Duration: 1M41
		
	Ellis Boss
		- Mobile Fort
		- Duration: 1M03
		
	Cerberus (replace sun) (Note there is a Pt1 and a Pt2!)
		- HIS BEHAVIOUR INSPIRED DISTRUST	(slightly wrong, it's INSPIRED US WITH, but that doesn't fit)
		- Stage: CERBERUS
		- Target: CERBERUS
		- Duration: 5M16
		
	Orn Base
		- Hunger Made Them Desperate
		- Stage: ORN BASE
		- Target: NO DATA
		- Duration: 3M51
		
	Orn Base Boss
		- Off Luck
		- Duration: 1M38
		
	Orn Core
		- Final Moment
		- Stage: ORN CORE
		- Target: CHA OS
		- Duration: 1M23
		
	Orn Boss (note there is a Pt1 and a Pt2!)
		- Be Menaced by Orn
		- Duration: 1M49
	
	Stage Clear
		- Stage Clear
		- Stage: CLEAR
		- Target: DESTROYED
		- Duration: 0M02
		
	Continue
		- Continue
		- Stage: CONTINUE
		- Target: NONE
		- Duration: 0M21

	Ending
		- A War Without the End
		- Stage: ALL CLEAR
		- Target: NONE
		- Duration: 1M04
		
	Staff Roll
		- Present
		- Stage: ALL CLEAR
		- Target: AWESOME STAFF
		- Duration: 3M07
		
	Game Over
		- Game Over
		- Stage: NO MORE
		- Target: NONE
		- Duration: 0M04
		
	Metal Squad (had pt1 and pt2!)
		- Metal Squad
		- STAGE: TFIV WALL
		- Target: EVIL CORE
		- Duration: 4M40
		
	TF4 Staff
		- STAND UP AGAINST MYSELF
		- Stage: TFIV STAFF
		- Target: MORE AWESOME STAFF
		- Duration: 1M49


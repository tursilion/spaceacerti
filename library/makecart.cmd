d:\work\ti\xb2gpl\release\xb2gpl.exe library.xb.txt 8099 library.bin > list.txt
copy /y /b ssa_ubergromHdr.bin + /b library.bin /b b1.bin
d:\work\setbinsize\release\setbinsize.exe b1.bin 512

copy /y /b b1.bin + /b ssc /b b2.bin
d:\work\setbinsize\release\setbinsize.exe b2.bin 7424

copy /y /b b2.bin + /b font.bin /b g8000_1

copy /y ssa gA000_1
d:\work\setbinsize\release\setbinsize.exe gA000_1 8192

copy /y ssb gC000_1
d:\work\setbinsize\release\setbinsize.exe gC000_1 8192

copy /y ssa2_a1 gE000_1
d:\work\setbinsize\release\setbinsize.exe gE000_1 8192

copy /y ssa2_a2 g8000_2
d:\work\setbinsize\release\setbinsize.exe g8000_2 8192

copy /y /b acer_c + /b ssa2_a3 /b gA000_2
d:\work\setbinsize\release\setbinsize.exe gA000_2 8192

copy /y acer_p gC000_2
d:\work\setbinsize\release\setbinsize.exe gC000_2 8192

copy /y sse b1.bin
d:\work\setbinsize\release\setbinsize.exe b1.bin 5120
copy /y /b b1.bin + /b DEMQ /b gE000_2
d:\work\setbinsize\release\setbinsize.exe gE000_2 8192

copy /y storyread1 b1.bin
d:\work\setbinsize\release\setbinsize.exe b1.bin -134
d:\work\setbinsize\release\setbinsize.exe b1.bin 2304

copy /y story2read1 b2.bin
d:\work\setbinsize\release\setbinsize.exe b2.bin -134
d:\work\setbinsize\release\setbinsize.exe b2.bin 2304

copy /y /b b1.bin + /b b2.bin /b g6000_3
d:\work\setbinsize\release\setbinsize.exe g6000_3 8192

copy /y ssd g8000_3
d:\work\setbinsize\release\setbinsize.exe g8000_3 8192

copy /y story2.bin gA000_3
d:\work\setbinsize\release\setbinsize.exe gA000_3 8192

copy /y story1a.bin gC000_3
d:\work\setbinsize\release\setbinsize.exe gC000_3 8192

copy /y story1b.bin gE000_3
d:\work\setbinsize\release\setbinsize.exe gE000_3 8192

@rem just using gromcfg to assemble the UberGROM data
@rem Then we can concatenate the AVR program after building

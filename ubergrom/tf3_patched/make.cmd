@rem music
copy /y sounds\TF3TI00.sbf .
d:\work\setbinsize\release\setbinsize TF3TI00.sbf 8192
copy /y sounds\TF3TI01.sbf .
d:\work\setbinsize\release\setbinsize TF3TI01.sbf 8192
copy /y sounds\TF3TI02.sbf .
d:\work\setbinsize\release\setbinsize TF3TI02.sbf 8192
copy /y sounds\TF3TI03.sbf .
d:\work\setbinsize\release\setbinsize TF3TI03.sbf 8192
copy /y sounds\TF3TI04.sbf .
d:\work\setbinsize\release\setbinsize TF3TI04.sbf 8192
copy /y sounds\TF3TI05.sbf .
d:\work\setbinsize\release\setbinsize TF3TI05.sbf 8192
copy /y sounds\TF3TI06.sbf .
d:\work\setbinsize\release\setbinsize TF3TI06.sbf 8192
copy /y sounds\TF3TI07.sbf .
d:\work\setbinsize\release\setbinsize TF3TI07.sbf 8192
copy /y sounds\TF3TI08.sbf .
d:\work\setbinsize\release\setbinsize TF3TI08.sbf 8192
copy /y sounds\TF3TI09.sbf .
d:\work\setbinsize\release\setbinsize TF3TI09.sbf 8192
copy /y sounds\TF3TI10.sbf .
d:\work\setbinsize\release\setbinsize TF3TI10.sbf 8192
copy /y sounds\TF3TI11.sbf .
d:\work\setbinsize\release\setbinsize TF3TI11.sbf 8192
copy /y sounds\TF3TI12.sbf .
d:\work\setbinsize\release\setbinsize TF3TI12.sbf 8192
copy /y sounds\TF3TI13.sbf .
d:\work\setbinsize\release\setbinsize TF3TI13.sbf 8192
copy /y sounds\TF3TI14.sbf .
d:\work\setbinsize\release\setbinsize TF3TI14.sbf 8192

@rem graphics
python D:\tools\xdt99\xas99.py -R magellan\map_boss_unknown.a99 -o map_boss_unknown.obj
@if errorlevel 1 goto error
d:\work\ti\tiobj2bin\release\tiobj2bin map_boss_unknown.obj map_boss_unknown.bin -raw -block
d:\work\setbinsize\release\setbinsize map_boss_unknown.bin 8192

python D:\tools\xdt99\xas99.py -R magellan\map_bosscerberus.a99 -o map_bosscerberus.obj
@if errorlevel 1 goto error
d:\work\ti\tiobj2bin\release\tiobj2bin map_bosscerberus.obj map_bosscerberus.bin -raw -block
d:\work\setbinsize\release\setbinsize map_bosscerberus.bin 8192

python D:\tools\xdt99\xas99.py -R magellan\map_bossfish.a99 -o map_bossfish.obj
@if errorlevel 1 goto error
d:\work\ti\tiobj2bin\release\tiobj2bin map_bossfish.obj map_bossfish.bin -raw -block
d:\work\setbinsize\release\setbinsize map_bossfish.bin 8192

python D:\tools\xdt99\xas99.py -R magellan\map_bossfort.a99 -o map_bossfort.obj
@if errorlevel 1 goto error
d:\work\ti\tiobj2bin\release\tiobj2bin map_bossfort.obj map_bossfort.bin -raw -block
d:\work\setbinsize\release\setbinsize map_bossfort.bin 8192

python D:\tools\xdt99\xas99.py -R magellan\map_bossgargoyle.a99 -o map_bossgargoyle.obj
@if errorlevel 1 goto error
d:\work\ti\tiobj2bin\release\tiobj2bin map_bossgargoyle.obj map_bossgargoyle.bin -raw -block
d:\work\setbinsize\release\setbinsize map_bossgargoyle.bin 8192

python D:\tools\xdt99\xas99.py -R magellan\map_bosslobster.a99 -o map_bosslobster.obj
@if errorlevel 1 goto error
d:\work\ti\tiobj2bin\release\tiobj2bin map_bosslobster.obj map_bosslobster.bin -raw -block
d:\work\setbinsize\release\setbinsize map_bosslobster.bin 8192

python D:\tools\xdt99\xas99.py -R magellan\map_bossvulcan.a99 -o map_bossvulcan.obj
@if errorlevel 1 goto error
d:\work\ti\tiobj2bin\release\tiobj2bin map_bossvulcan.obj map_bossvulcan.bin -raw -block
d:\work\setbinsize\release\setbinsize map_bossvulcan.bin 8192

python D:\tools\xdt99\xas99.py -R magellan\map_briefing.a99 -o map_briefing.obj
@if errorlevel 1 goto error
d:\work\ti\tiobj2bin\release\tiobj2bin map_briefing.obj map_briefing.bin -raw -block
d:\work\setbinsize\release\setbinsize map_briefing.bin 8192

python D:\tools\xdt99\xas99.py -R magellan\map_briefingornbase.a99 -o map_briefingornbase.obj
@if errorlevel 1 goto error
d:\work\ti\tiobj2bin\release\tiobj2bin map_briefingornbase.obj map_briefingornbase.bin -raw -block
d:\work\setbinsize\release\setbinsize map_briefingornbase.bin 8192

python D:\tools\xdt99\xas99.py -R magellan\map_titleThird1.a99 -o map_titleThird1.obj
@if errorlevel 1 goto error
d:\work\ti\tiobj2bin\release\tiobj2bin map_titleThird1.obj map_titleThird1.bin -raw -block
d:\work\setbinsize\release\setbinsize map_titleThird1.bin 8192

python D:\tools\xdt99\xas99.py -R magellan\map_titleThird2.a99 -o map_titleThird2.obj
@if errorlevel 1 goto error
d:\work\ti\tiobj2bin\release\tiobj2bin map_titleThird2.obj map_titleThird2.bin -raw -block
d:\work\setbinsize\release\setbinsize map_titleThird2.bin 8192

python D:\tools\xdt99\xas99.py -R magellan\map_titleThird3.a99 -o map_titleThird3.obj
@if errorlevel 1 goto error
d:\work\ti\tiobj2bin\release\tiobj2bin map_titleThird3.obj map_titleThird3.bin -raw -block
d:\work\setbinsize\release\setbinsize map_titleThird3.bin 8192

@rem code
python D:\tools\xdt99\xas99.py -R loader.a99 -L loader.lst -o loader.obj
@if errorlevel 1 goto error
d:\work\ti\tiobj2bin\release\tiobj2bin loader.obj loaderC.bin -raw -block
d:\work\setbinsize\release\setbinsize loaderC.bin 8192

python D:\tools\xdt99\xas99.py -R tf3.a99 -L tf3.lst -o tf3.obj
@if errorlevel 1 goto error
d:\work\ti\tiobj2bin\release\tiobj2bin tf3.obj tf3.bin -raw -block
d:\work\setbinsize\release\setbinsize tf3.bin 24576

@rem 256k cart - but warning, expects to run in the second half!
copy /b loaderC.bin ^
+ /b tf3.bin ^
+ /b TF3TI00.sbf ^
+ /b TF3TI01.sbf ^
+ /b TF3TI02.sbf ^
+ /b TF3TI03.sbf ^
+ /b TF3TI04.sbf ^
+ /b TF3TI05.sbf ^
+ /b TF3TI06.sbf ^
+ /b TF3TI07.sbf ^
+ /b TF3TI08.sbf ^
+ /b TF3TI09.sbf ^
+ /b TF3TI10.sbf ^
+ /b TF3TI11.sbf ^
+ /b map_boss_unknown.bin ^
+ /b map_bosscerberus.bin ^
+ /b map_bossfish.bin ^
+ /b map_bossfort.bin ^
+ /b map_bossgargoyle.bin ^
+ /b map_bosslobster.bin ^
+ /b map_bossvulcan.bin ^
+ /b map_briefing.bin ^
+ /b map_briefingornbase.bin ^
+ /b map_titleThird1.bin ^
+ /b map_titleThird2.bin ^
+ /b map_titleThird3.bin ^
+ /b TF3TI12.sbf ^
+ /b TF3TI13.sbf ^
+ /b TF3TI14.sbf ^
+ /b loaderC.bin ^
/b TF3_8.bin

goto :EOF

:error
@ECHO ***
@ECHO * Assembly failed
@echo ***

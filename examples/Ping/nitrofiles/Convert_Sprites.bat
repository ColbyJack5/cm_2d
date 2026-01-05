@echo off
cls

for %%F in (png\backgrounds\*.png) do (
    echo Converting %%F...
    "png2bmp.exe" "%%F"
)
move png\backgrounds\*.bmp bmp\backgrounds

del backgrounds\*.img
del backgrounds\*.map
del backgrounds\*.pal
del h\*.h
grit.exe bmp\backgrounds\*.bmp -g -gTFF00FF -m -mRtf -mLs -p -ftb
for /f "delims==" %%F in ('dir /b *.bin') do ren "%%~nxF" "%%~nF"
move *.img backgrounds
move *.map backgrounds
move *.pal backgrounds
move *.h h


for %%F in (png\sprites\*.png) do (
    echo Converting %%F...
    "png2bmp.exe" "%%F"
)
move png\sprites\*.bmp bmp\sprites

set /p PALETTE=Shared Palette name:

del sprites\*.img
del sprites\*.map
del sprites\*.pal
del h\*.h

grit.exe bmp\sprites\*.bmp -g -gTFF00FF -p -pS -ftb -O%PALETTE%

for /f "delims==" %%F in ('dir /b *.bin') do ren "%%~nxF" "%%~nF"
move *.img sprites
move *.pal sprites
del *.map
move *.h h
pause
exit
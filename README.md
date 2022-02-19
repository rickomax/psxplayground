##PSXPlayground (WIP)

PSXPlayground is an attempt to build an LUA scriptable engine that runs on desktop, web, and in the future, actual Playstation 1 (PSX) devices.

PSXPlayground uses:
Lua 5.4.3 (modified version to handle fixed-point math) (https://www.lua.org/download.html)
MaxRectsBinPack (https://github.com/juj/RectangleBinPack/blob/master/MaxRectsBinPack.h)
OpenAL-Soft (https://github.com/kcat/openal-soft)
PsyCross (https://github.com/OpenDriver2/PsyCross)
SDL2 (https://www.libsdl.org/download-2.0.php)

PSXPlayground uses code from the following projects:
TOMB5 (https://github.com/TOMB5/TOMB5)
REDRIVER2 (https://github.com/OpenDriver2/REDRIVER2) 
PSn00bSDK (https://github.com/Lameguy64/PSn00bSDK)
tinyobjloader (https://github.com/tinyobjloader/tinyobjloader)
bmp2tim (https://github.com/ColdSauce/psxsdk/blob/master/tools/bmp2tim.c)
wav2vag (https://github.com/ColdSauce/psxsdk/blob/master/tools/wav2vag.c)

The Sample Project (WIP) uses assets from Jacob Lenard:
https://www.instagram.com/jacoblenstar/
https://www.youtube.com/c/LenstarProductions

You can build PSXPlayground via CMAKE.

To run it, you must specify:
 - A LUA script filename
 - The window width
 - The window height
 - The fullscreen flag (1 for true or 0 for false)

You can test the SampleProject, by running the "RunMe.bat" inside the "SampleProject" folder inside your compilation folder under Windows. Other platforms can be tested using a similar SH script.

PSXPlayground expects the following functions in your LUA script:
 - init - This function is called when PSXPlayground launches
 - render - This functions is called every frame

As in the actual PSX API, PSXPlayground has methods to deal with Short Vectors (SV - used in rotations), and Long Vectors (LV - used in everything else)

The complete API is a work in progress.

A special thanks to all PSXDEV Network Discord server friends.
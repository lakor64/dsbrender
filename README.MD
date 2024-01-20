# DSC/BRender fork
The idea of this fork was to implement some remaining functionality that makes the BRender an actual game engine
rather than a simple 3D Graphics render, what I see very powerfull on the BRender is the software render 1990 grade.

Start date: 14-06-2023

## Current new features
- Windowing API is now available, relevant functions are included in host.h and host_p.h
- Input API (Mouse+keyboard) is now available, relevant functions are included in host.h and host_p.h
- Introduction of the cmake files
- Built with also modern compilers

## Configuring

### Windows NT6+ 32-bit
`cmake -G "Visual Studio 17 2022" -A Win32`

### Windows 3x/9x (Open Watcom 2.0)
`cmake -G "Watcom WMake" -D CMAKE_SYSTEM_NAME=Windows`

### DOS with DOS extender (Open Watcom 2.0)
`cmake -G "Watcom WMake" -D CMAKE_SYSTEM_NAME=DOS`

### OS/2 (Open Watcom 2.0)
`cmake -G "Watcom WMake" -D CMAKE_SYSTEM_NAME=OS2`

## Special thanks

- https://github.com/foone/BRender-v1.3.2
Original BRender repository which this fork is based

- https://github.com/crocguy0688/CrocDE-BRender
For `mkdrv.pl`,cmake inspiration, softrnd fnt.c

- BRender SDK 25 (Eval 1.2)
minzb example, dfwlib which I decompiled

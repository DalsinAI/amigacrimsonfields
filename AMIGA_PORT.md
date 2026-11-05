Crimson Fields for AmigaOS 3.x
==============================

A port of Crimson Fields 0.5.3 (Jens Granseuer's turn-based tactical war
game in the tradition of Battle Isle, http://crimson.seul.org, from its
release tarball crimson-0.5.3.tar.bz2, SHA-256
d59858c05e340367c61c8ca1dd00c36642a0c56d10d1d9a1626c5ba7d88b40d6) for
AmigaOS 3.2 on a 68040 or 68060 with an FPU and an RTG screen, on
GFOpenGPU's SDL 2 (with SDL2_ttf and SDL2_mixer).

Status: plays in the AmiCradle lab (see "Testing" at the end).


What you need
-------------

- A 68040 or 68060 with an FPU (not an LC040 or EC040), AmigaOS 3.2, an
  RTG card (or an AmiCradle instance).
- GFOpenGPU with LIBS:GFOpenGPU/SDL2.module and SDL2_mixer.module
  (SDL2_ttf, with FreeType, is built into the program).
- About 4 MB of disk and 16 MB of free memory.

Everything the game needs is in the drawer: the game's graphics, maps,
texts, sounds, music and font are all free (see Licences.txt).


How to play
-----------

Double-click CrimsonFields in its drawer, or from a Shell:

    cd Work:Games/CrimsonFields
    CrimsonFields [--width W] [--height H] [--fullscreen 1|0] [--level FILE]

It opens an 800x600 window on the Workbench (--fullscreen 1 a screen of
its own; Alt+Return switches). The title menu picks the game type (Hot
Seat, against the computer, by e-mail), the maps (skirmishes, the two
campaigns, the saved games) and the handicap; Start begins.

- The mouse does almost everything: click a unit to choose it, click a
  hex to move it there or a red-marked enemy to attack. The right button
  (or the unit's own menu) shows what a unit can do.
- F1 the game menu (End Turn, Map, Objectives, Level Info, Options,
  Save..., Surrender, Quit...), F2 the next unit, Space chooses the
  unit under the cursor; the cursor keys and keypad move the cursor.
  The keys can be changed under Options.
- F11 turns the sound effects on and off.
- Right Amiga+Q quits at once from anywhere; the settings are kept
  (the game itself is not saved: use F1, Save... first).

The settings (crimsonrc) and saved games (games/*.sav) are kept in the
drawer's User drawer; the installer leaves User alone when it updates
the drawer.


Building
--------

The os32-gcc16-open6 stove (GFOpenAmigaGCC 16.2.0-open6 with the
GFOpenGPU 0.8 kit), and on the build machine (Linux) a C++ compiler,
SDL 2's development files (sdl2-config) and zlib's, for upstream's data
tools; and the network the first time (zlib's source tarball):

    make -f amiga/Makefile.amigaos -j4 dist STOVE=/path/to/os32-gcc16-open6

STOVE defaults to ~/AmiCradle/stoves/os32-gcc16-open6. amiga/deps/
build-deps.sh fetches zlib 1.3.1 into amiga/deps/cache (SHA-256 checked)
and builds it static; amiga/build-data.sh builds upstream's mkdatafile,
mktileset, mkunitset, mklocale and cfed for the build machine and makes
the data with them in build/hostdata. Output: build/amigaos/crimson;
'dist' makes dist/CrimsonFields/ (CrimsonFields and its data, ReadMe,
ReadMe-crimson, COPYING.txt, Licences.txt, with GlowIcons),
dist/CrimsonFields.info and dist/Install-CrimsonFields with its icon. The
icons are made from amiga/icons/icons.json and the installer from
amiga/installer.json with AmiCradle's scripts/port_icons.py and
port_installer.py.


What was changed
----------------

Crimson Fields 0.5.3 is written for SDL 1.2, which GFOpenGPU has only as
sdl12-compat, without SDL_ttf or SDL_mixer for 1.2. So it runs on SDL 2:

- src/common/sdlcompat.h and sdlcompat.cpp (new; sdlcompat.h is given to
  every file with -include): the SDL 1.2 calls the game makes, on SDL 2.
  SDL_SetVideoMode opens a window and returns its window surface;
  SDL_UpdateRect updates the window surface's rectangle; SDL_SetAlpha is
  the surface's alpha and blend mode; SDL_DisplayFormat converts to the
  window's format without alpha and keeps the colour key and alpha (SDL
  2's conversion lost a 16-bit surface's key, which drew the fog's
  corners); the window manager calls; the 1.2 key names.
- src/common/view.cpp: each event goes through sdlcompat_translate: the
  wheel becomes buttons 4 and 5, each key press gets its character (as
  1.2's keysym.unicode, which the text boxes use), text events are
  dropped; the mouse motion merging uses SDL 2's PeepEvents.
- src/common/surface.cpp: the palette is set with SDL_SetPaletteColors
  and the colour key read with SDL_GetColorKey. textbox.cpp/h: the
  character from sdlcompat; key codes as int. SDL_zlib.c: SDL 2's RWops
  callbacks. globals.h: 32-bit surfaces, like the window. src/cf/game.cpp:
  SDL 2's key codes are kept whole (upstream masked them to 9 bits).
- src/common/fileio.cpp: no HOME on AmigaOS: the data is read from
  PROGDIR:, the settings and saves go in PROGDIR:User/ (and its games/);
  no "/" after a ":" ("PROGDIR:/x" is the parent drawer); drawers are made
  without a trailing "/".
- src/cf/main.cpp: Right Amiga+Q quits, through upstream's own exit (the
  settings are saved, SDL closed).
- amiga/build-data.sh: upstream's data tools for the build machine (on
  its SDL 2 through sdlcompat.h; amiga/host-include/SDL_ttf.h gives them
  SDL_ttf's one type) and the data made with them, as upstream's
  Makefiles do.
- amiga/Makefile.amigaos, amiga/deps/build-deps.sh (zlib), amiga/stack.c
  (512 KB of stack and a $VER string).

Big-endian: the data files and saved games are read and written a byte
at a time as little-endian (src/common/fileio.cpp, SDL_ReadLE16 and
friends), so the files made on the build machine are read as they are;
the maps, units and saved games load in the lab.

Frame pacing: the game waits for events (SDL_WaitEvent) and draws only
what changes, so it does not spin; its animations time themselves with
SDL_Delay and SDL_GetTicks.


Known limits
------------

- No network play (built with DISABLE_NETWORK: there is no SDL_net for
  the kit). Hot seat, play by e-mail (the saved game file) and play
  against the computer all work.
- The computer's turns are worked out by the 68k; in the lab its first
  turn on Across the Omyar Gorge took under ten seconds, and bigger maps
  will take longer.
- The level editor (CoMET) and the Battle Isle map converter are not
  built.


Licence
-------

Crimson Fields is under the GNU GPL, version 2 or later (COPYING.txt);
the port's files too. The data, the music and the font are covered in
Licences.txt.


Testing
-------

Tested in an AmiCradle lab (a copy of Instance-32: AmigaOS 3.2, 68040,
RTG; the lab is muted, so the sound could not be heard), 5 November 2026:

- The title picture and the title menu draw in the right colours in an
  800x600 window. The skirmish list is filled from levels/, and the map
  preview of Across the Omyar Gorge draws.
- Started that map against the computer. The briefing showed, and
  "Turn 1" took the keyboard (Return). The hex map, the units and the
  fog of war drew correctly once the colour key fix was in (before it,
  dark stripes showed between the hex columns).
- Chose the artillery and moved it two hexes along the road. Opened the
  game menu with F1, saved (User/games/OmyarGorge.sav, 12 KB) and ended
  the turn. The computer played its turn and turn 2 came.
- Right Amiga+Q quit at once to the Workbench with no screen left, and
  User/crimsonrc was written.
- Started the program again. Under Saved Games, "OmyarGorge (1, Turn 1)"
  loaded with the artillery where it had been moved. Then quit through
  F1, Quit..., Yes: back to the Workbench cleanly.
- Alt+Return switched to an 800x600 screen of its own and back to the
  window, the title menu redrawn each time. Right Amiga+Q then quit with
  no screen left behind.

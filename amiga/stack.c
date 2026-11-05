/* A Shell gives 4 KB of stack; Crimson Fields' C++ (the computer player's
 * path searches most of all) wants far more. libnix's startup sets this up.
 * Copyright (C) 2026 AmiCradle Team. GPL 2 or later, like Crimson Fields. */
unsigned long __stack = 512 * 1024;
static const char version[] __attribute__((used)) = "$VER: CrimsonFields 0.5.3 Amiga (05.11.2026)";

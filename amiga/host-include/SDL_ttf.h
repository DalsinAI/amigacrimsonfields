/* For amiga/build-data.sh only: the host tools include font.h through the
 * window headers but never draw text, so SDL_ttf's type is all they need
 * (the build machine need not have SDL2_ttf).
 * Copyright (C) 2026 AmiCradle Team. GPL 2 or later, like Crimson Fields. */
#ifndef CF_HOST_SDL_TTF_STUB_H
#define CF_HOST_SDL_TTF_STUB_H
typedef struct _TTF_Font TTF_Font;
#endif

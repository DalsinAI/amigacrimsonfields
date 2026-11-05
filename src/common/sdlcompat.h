// sdlcompat.h: the SDL 1.2 calls Crimson Fields makes, on SDL 2.
//
// Crimson Fields 0.5.3 is written for SDL 1.2. This header (given to the
// compiler with -include, so every file sees it) maps the 1.2 names it
// uses onto SDL 2: the display is an SDL 2 window and its window surface
// (SDL_SetVideoMode, SDL_UpdateRect), per-surface alpha (SDL_SetAlpha),
// SDL_DisplayFormat, the window manager calls and the 1.2 key names.
// With SDL 1.2 it does nothing. sdlcompat.cpp holds the functions;
// View::FetchEvent turns SDL 2's wheel and text events into what the 1.2
// code expects (sdlcompat_translate).
//
// Copyright (C) 2026 AmiCradle Team. GPL 2 or later, like Crimson Fields.

#ifndef _INCLUDE_SDLCOMPAT_H
#define _INCLUDE_SDLCOMPAT_H

#include "SDL.h"

#if SDL_VERSION_ATLEAST(2,0,0)

#ifdef __cplusplus
extern "C" {
#endif

typedef SDL_Keycode SDLKey;
typedef SDL_Keymod SDLMod;
typedef SDL_Keysym SDL_keysym;

// SDL 1.2's surface and video flags, as Crimson Fields passes them on
#define SDL_SWSURFACE   0x00000000
#define SDL_HWSURFACE   0x00000001
#define SDL_SRCCOLORKEY 0x00001000
#define SDL_SRCALPHA    0x00010000
#define SDL_FULLSCREEN  0x80000000

#define SDL_DEFAULT_REPEAT_DELAY    500
#define SDL_DEFAULT_REPEAT_INTERVAL 30

#define SDL_BUTTON_WHEELUP   4
#define SDL_BUTTON_WHEELDOWN 5

#define SDLK_KP0 SDLK_KP_0
#define SDLK_KP1 SDLK_KP_1
#define SDLK_KP2 SDLK_KP_2
#define SDLK_KP3 SDLK_KP_3
#define SDLK_KP4 SDLK_KP_4
#define SDLK_KP5 SDLK_KP_5
#define SDLK_KP6 SDLK_KP_6
#define SDLK_KP7 SDLK_KP_7
#define SDLK_KP8 SDLK_KP_8
#define SDLK_KP9 SDLK_KP_9

SDL_Surface *SDL_SetVideoMode(int w, int h, int bpp, Uint32 flags);
void SDL_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h);
SDL_Surface *SDL_DisplayFormat(SDL_Surface *s);
int SDL_SetAlpha(SDL_Surface *s, Uint32 flags, Uint8 alpha);
void SDL_WM_SetCaption(const char *title, const char *icon);
void SDL_WM_SetIcon(SDL_Surface *icon, Uint8 *mask);
int SDL_WM_ToggleFullScreen(SDL_Surface *screen);
int SDL_WM_IconifyWindow(void);
int SDL_EnableUNICODE(int enable);
int SDL_EnableKeyRepeat(int delay, int interval);
SDL_Rect **SDL_ListModes(void *format, Uint32 flags);
Uint32 sdlcompat_colorkey(SDL_Surface *s);
int sdlcompat_fullscreen(void);

// Fixes up an event from SDL_WaitEvent/SDL_PollEvent for the 1.2 code:
// a wheel step becomes a button 4/5 press at the pointer, a key press gets
// its character (sdlcompat_unicode, read the way 1.2's keysym.unicode
// was). Returns 0 for events to drop (text input, already used).
int sdlcompat_translate(SDL_Event *event);
extern Uint16 sdlcompat_unicode;

#ifdef __cplusplus
}
#endif

#endif /* SDL 2 */
#endif /* _INCLUDE_SDLCOMPAT_H */

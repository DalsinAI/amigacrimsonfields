// sdlcompat.cpp: the SDL 1.2 calls Crimson Fields makes, on SDL 2
// (see sdlcompat.h).
//
// Copyright (C) 2026 AmiCradle Team. GPL 2 or later, like Crimson Fields.

#include "sdlcompat.h"

#if SDL_VERSION_ATLEAST(2,0,0)

#include <string.h>

static SDL_Window *window = NULL;
static SDL_Surface *window_icon = NULL;
static char window_title[128] = "Crimson Fields";

Uint16 sdlcompat_unicode = 0;

SDL_Surface *SDL_SetVideoMode( int w, int h, int bpp, Uint32 flags ) {
  (void)bpp;
  Uint32 wflags = (flags & SDL_FULLSCREEN) ? SDL_WINDOW_FULLSCREEN : 0;

  if ( !window ) {
    window = SDL_CreateWindow( window_title, SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, w, h, wflags );
    if ( !window ) return NULL;
    if ( window_icon ) SDL_SetWindowIcon( window, window_icon );
    SDL_StartTextInput();
  } else {
    SDL_SetWindowFullscreen( window, 0 );
    SDL_SetWindowSize( window, w, h );
    if ( wflags ) SDL_SetWindowFullscreen( window, wflags );
  }
  return SDL_GetWindowSurface( window );
}

void SDL_UpdateRect( SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h ) {
  if ( !window ) return;
  if ( (w == 0) && (h == 0) ) {
    SDL_UpdateWindowSurface( window );
    return;
  }
  // clip to the window surface, as 1.2 did
  if ( x < 0 ) { w += x; x = 0; }
  if ( y < 0 ) { h += y; y = 0; }
  if ( screen ) {
    if ( x + w > screen->w ) w = screen->w - x;
    if ( y + h > screen->h ) h = screen->h - y;
  }
  if ( (w <= 0) || (h <= 0) ) return;
  SDL_Rect r = { x, y, w, h };
  SDL_UpdateWindowSurfaceRects( window, &r, 1 );
}

// The window's own format, without an alpha channel: per-surface alpha
// (SDL_SetAlpha) then works as it did on 1.2.
static Uint32 display_format( void ) {
  SDL_Surface *ws = window ? SDL_GetWindowSurface( window ) : NULL;
  if ( !ws ) return SDL_PIXELFORMAT_RGB888;
  switch ( ws->format->format ) {
  case SDL_PIXELFORMAT_ARGB8888: return SDL_PIXELFORMAT_RGB888;
  case SDL_PIXELFORMAT_RGBA8888: return SDL_PIXELFORMAT_RGBX8888;
  case SDL_PIXELFORMAT_ABGR8888: return SDL_PIXELFORMAT_BGR888;
  case SDL_PIXELFORMAT_BGRA8888: return SDL_PIXELFORMAT_BGRX8888;
  default: return ws->format->format;
  }
}

SDL_Surface *SDL_DisplayFormat( SDL_Surface *s ) {
  return SDL_ConvertSurfaceFormat( s, display_format(), 0 );
}

int SDL_SetAlpha( SDL_Surface *s, Uint32 flags, Uint8 alpha ) {
  if ( !s ) return -1;
  if ( flags & SDL_SRCALPHA ) {
    SDL_SetSurfaceAlphaMod( s, alpha );
    SDL_SetSurfaceBlendMode( s, SDL_BLENDMODE_BLEND );
  } else {
    SDL_SetSurfaceAlphaMod( s, SDL_ALPHA_OPAQUE );
    SDL_SetSurfaceBlendMode( s, SDL_BLENDMODE_NONE );
  }
  return 0;
}

void SDL_WM_SetCaption( const char *title, const char *icon ) {
  (void)icon;
  if ( title ) {
    strncpy( window_title, title, sizeof(window_title) - 1 );
    window_title[sizeof(window_title) - 1] = '\0';
  }
  if ( window ) SDL_SetWindowTitle( window, window_title );
}

void SDL_WM_SetIcon( SDL_Surface *icon, Uint8 *mask ) {
  (void)mask;
  // a copy: 1.2 took the picture at once, and the caller frees it
  if ( window_icon ) SDL_FreeSurface( window_icon );
  window_icon = icon ? SDL_ConvertSurface( icon, icon->format, 0 ) : NULL;
  if ( window && window_icon ) SDL_SetWindowIcon( window, window_icon );
}

int SDL_WM_ToggleFullScreen( SDL_Surface *screen ) {
  (void)screen;
  if ( !window ) return 0;
  Uint32 fs = SDL_GetWindowFlags( window ) & SDL_WINDOW_FULLSCREEN;
  return SDL_SetWindowFullscreen( window, fs ? 0 : SDL_WINDOW_FULLSCREEN ) == 0;
}

int sdlcompat_fullscreen( void ) {
  return window && (SDL_GetWindowFlags( window ) & SDL_WINDOW_FULLSCREEN);
}

int SDL_WM_IconifyWindow( void ) {
  if ( window ) SDL_MinimizeWindow( window );
  return 1;
}

int SDL_EnableUNICODE( int enable ) {
  (void)enable;
  return 1;           // characters always come (sdlcompat_translate)
}

int SDL_EnableKeyRepeat( int delay, int interval ) {
  (void)delay; (void)interval;
  return 0;           // SDL 2 repeats keys itself
}

SDL_Rect **SDL_ListModes( void *format, Uint32 flags ) {
  (void)format; (void)flags;
  return (SDL_Rect **)-1;   // any size: the game offers its standard ones
}

Uint32 sdlcompat_colorkey( SDL_Surface *s ) {
  Uint32 key = 0;
  SDL_GetColorKey( s, &key );
  return key;
}

int sdlcompat_translate( SDL_Event *event ) {
  switch ( event->type ) {
  case SDL_TEXTINPUT:
    return 0;         // read with its key press below

  case SDL_MOUSEWHEEL: {
    int x, y;
    if ( event->wheel.y == 0 ) return 0;
    Uint8 button = (event->wheel.y > 0) ? SDL_BUTTON_WHEELUP : SDL_BUTTON_WHEELDOWN;
    SDL_GetMouseState( &x, &y );
    memset( &event->button, 0, sizeof(event->button) );
    event->type = SDL_MOUSEBUTTONDOWN;
    event->button.button = button;
    event->button.state = SDL_PRESSED;
    event->button.x = x;
    event->button.y = y;
    return 1;
  }

  case SDL_MOUSEBUTTONDOWN:
  case SDL_MOUSEBUTTONUP:
    // SDL 2 has no buttons 4 and 5 (the wheel is above); its X1/X2
    // buttons would be taken for them
    return event->button.button <= SDL_BUTTON_RIGHT;

  case SDL_KEYDOWN: {
    SDL_Event text;
    SDL_Keycode sym = event->key.keysym.sym;

    sdlcompat_unicode = 0;
    switch ( sym ) {
    case SDLK_BACKSPACE: sdlcompat_unicode = '\b'; break;
    case SDLK_DELETE:    sdlcompat_unicode = 127;  break;
    case SDLK_RETURN:
    case SDLK_KP_ENTER:  sdlcompat_unicode = '\r'; break;
    case SDLK_TAB:       sdlcompat_unicode = '\t'; break;
    case SDLK_ESCAPE:    sdlcompat_unicode = 27;   break;
    default:
      // the character SDL 2 sends with this key, if any (Latin-1 only)
      SDL_PumpEvents();
      if ( SDL_PeepEvents( &text, 1, SDL_GETEVENT, SDL_TEXTINPUT, SDL_TEXTINPUT ) > 0 ) {
        const unsigned char *t = (const unsigned char *)text.text.text;
        if ( t[0] < 0x80 ) sdlcompat_unicode = t[0];
        else if ( (t[0] & 0xe0) == 0xc0 ) {
          Uint16 c = ((t[0] & 0x1f) << 6) | (t[1] & 0x3f);
          if ( c < 0x100 ) sdlcompat_unicode = c;
        }
      }
    }
    return 1;
  }

  default:
    return 1;
  }
}

#endif /* SDL 2 */

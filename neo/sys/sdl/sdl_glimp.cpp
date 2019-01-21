/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 BFG Edition GPL Source Code ("Doom 3 BFG Edition Source Code").  

Doom 3 BFG Edition Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 BFG Edition Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 BFG Edition Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 BFG Edition Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 BFG Edition Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#pragma hdrstop
#include "../../idlib/precompiled.h"

#ifdef ID_WIN
#include "SDL_syswm.h"
#define	WINDOW_STYLE (WS_OVERLAPPED|WS_BORDER|WS_CAPTION|WS_VISIBLE|WS_THICKFRAME|WS_SYSMENU)
#endif

#include "sdl_local.h"
#include "../win32/rc/doom_resource.h"
#include "../../renderer/tr_local.h"

idCVar r_useOpenGL32( "r_useOpenGL32", "1", CVAR_INTEGER, "0 = OpenGL 2.0, 1 = OpenGL 3.2 compatibility profile, 2 = OpenGL 3.2 core profile", 0, 2 );

/*
====================
GLimp_TestSwapBuffers
====================
*/
void GLimp_TestSwapBuffers( const idCmdArgs &args ) {
	idLib::Printf( "GLimp_TimeSwapBuffers\n" );
	static const int MAX_FRAMES = 5;
	uint64	timestamps[MAX_FRAMES];
	qglDisable( GL_SCISSOR_TEST );

	int frameMilliseconds = 16;
	for ( int swapInterval = 1 ; swapInterval >= -1 ; swapInterval-- ) {
		SDL_GL_SetSwapInterval( swapInterval );
		for ( int i = 0 ; i < MAX_FRAMES ; i++ ) {
			if ( swapInterval == -1 ) {
				Sys_Sleep( frameMilliseconds );
			}
			if ( i & 1 ) {
				qglClearColor( 0, 1, 0, 1 );
			} else {
				qglClearColor( 1, 0, 0, 1 );
			}
			qglClear( GL_COLOR_BUFFER_BIT );
			SDL_GL_SwapWindow( sdl.window );
			qglFinish();
			timestamps[i] = Sys_Microseconds();
		}

		idLib::Printf( "\nswapinterval %i\n", swapInterval );
		for ( int i = 1 ; i < MAX_FRAMES ; i++ ) {
			idLib::Printf( "%i microseconds\n", (int)(timestamps[i] - timestamps[i-1]) );
		}
	}
}

/*
====================
GLimp_SetGamma

The renderer calls this when the user adjusts r_gamma or r_brightness
====================
*/
void GLimp_SetGamma( unsigned short red[256], unsigned short green[256], unsigned short blue[256] ) {
	if ( SDL_SetWindowGammaRamp( sdl.window, red, green, blue ) == -1 ) {
		common->Printf( "WARNING: SDL_SetWindowGammaRamp failed.\n" );
	}
}

/*
====================
GetDisplayCoordinates
====================
*/
static bool GetDisplayCoordinates( const int deviceNum, int & x, int & y, int & width, int & height ) {
	idStr deviceName = idStr( SDL_GetDisplayName( deviceNum ) );
	if ( deviceName.Length() == 0 ) {
		return false;
	}

	SDL_Rect r;
	if ( SDL_GetDisplayBounds( deviceNum, &r ) != 0 ) {
		common->Printf( "SDL_GetDisplayBounds failed: %s", SDL_GetError() );
		return false;
	}

	common->Printf( "display device: %i\n", deviceNum );
	common->Printf( "  DeviceName  : %s\n", deviceName.c_str() );
	common->Printf( "                 x: %i\n", r.x );
	common->Printf( "                 y: %i\n", r.y );
	common->Printf( "                 w: %i\n", r.w );
	common->Printf( "                 h: %i\n", r.h );

	x = r.x;
	y = r.y;
	width = r.w;
	height = r.h;

	return true;
}

/*
====================
DumpAllDisplayDevices
====================
*/
void DumpAllDisplayDevices() {
	common->Printf( "\n" );
	for ( int deviceNum = 0 ; ; deviceNum++ ) {
		if ( deviceNum >= SDL_GetNumVideoDisplays() ) {
			break;
		}

		common->Printf( "display device: %i\n", deviceNum );
		common->Printf( "  DisplayName : %s\n", SDL_GetDisplayName( deviceNum ) );

		SDL_DisplayMode displayMode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };

		if ( SDL_GetCurrentDisplayMode( deviceNum, &displayMode ) != 0 ) {
			common->Printf( "ERROR:  SDL_GetCurrentDisplayMode failed!\n" );
		}
		common->Printf( "      -------------------\n" );
		common->Printf( "      CurrentDisplayMode\n" );
		common->Printf( "      format              : %s\n", SDL_GetPixelFormatName( displayMode.format ) );
		common->Printf( "      w                   : %i\n", displayMode.w );
		common->Printf( "      h                   : %i\n", displayMode.h );
		common->Printf( "      refresh_rate        : %i\n", displayMode.refresh_rate );

		for ( int modeNum = 0 ; ; modeNum++ ) {
			if ( SDL_GetDisplayMode( deviceNum, modeNum, &displayMode ) != 0 ) {
				break;
			}

			if ( SDL_BITSPERPIXEL( displayMode.format ) != 24 ) {
				continue;
			}
			if ( ( displayMode.refresh_rate != 60 ) && ( displayMode.refresh_rate != 120 ) ) {
				continue;
			}
			if ( displayMode.h < 720 ) {
				continue;
			}
			common->Printf( "      -------------------\n" );
			common->Printf( "      modeNum             : %i\n", modeNum );
			common->Printf( "      format              : %s\n", SDL_GetPixelFormatName( displayMode.format ) );
			common->Printf( "      w                   : %i\n", displayMode.w );
			common->Printf( "      h                   : %i\n", displayMode.h );
			common->Printf( "      refresh_rate        : %i\n", displayMode.refresh_rate );
		}
	}
	common->Printf( "\n" );
}

/*
====================
R_GetModeListForDisplay
====================
*/
bool R_GetModeListForDisplay( const int requestedDisplayNum, idList<vidMode_t> & modeList ) {
	modeList.Clear();

	bool	verbose = false;

	for ( int displayNum = requestedDisplayNum; ; displayNum++ ) {
		if ( displayNum >= SDL_GetNumVideoDisplays() ) {
			break;
		}

		if ( verbose ) {
			common->Printf( "display device: %i\n", displayNum );
			common->Printf( "  DisplayName : %s\n", SDL_GetDisplayName( displayNum ) );
		}

		SDL_DisplayMode displayMode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };

		for ( int modeNum = 0 ; ; modeNum++ ) {
			if ( SDL_GetDisplayMode( displayNum, modeNum, &displayMode ) != 0 ) {
				break;
			}

			if ( SDL_BITSPERPIXEL( displayMode.format ) != 24 ) {
				continue;
			}
			if ( ( displayMode.refresh_rate != 60 ) && ( displayMode.refresh_rate != 120 ) ) {
				continue;
			}
			if ( displayMode.h < 720 ) {
				continue;
			}
			if ( verbose ) {
				common->Printf( "      -------------------\n" );
				common->Printf( "      modeNum             : %i\n", modeNum );
				common->Printf( "      format              : %s\n", SDL_GetPixelFormatName( displayMode.format ) );
				common->Printf( "      w                   : %i\n", displayMode.w );
				common->Printf( "      h                   : %i\n", displayMode.h );
				common->Printf( "      refresh_rate        : %i\n", displayMode.refresh_rate );
			}
			vidMode_t mode;
			mode.width = displayMode.w;
			mode.height = displayMode.h;
			mode.displayHz = displayMode.refresh_rate;
			modeList.AddUnique( mode );
		}
		if ( modeList.Num() > 0 ) {

			class idSort_VidMode : public idSort_Quick< vidMode_t, idSort_VidMode > {
			public:
				int Compare( const vidMode_t & a, const vidMode_t & b ) const {
					int wd = a.width - b.width;
					int hd = a.height - b.height;
					int fd = a.displayHz - b.displayHz;
					return ( hd != 0 ) ? hd : ( wd != 0 ) ? wd : fd;
				}
			};

			// sort with lowest resolution first
			modeList.SortWithTemplate( idSort_VidMode() );

			return true;
		}
	}
	// Never gets here
	return false;
}

/*
====================
GLimp_GetWindowDimensions
====================
*/
static bool GLimp_GetWindowDimensions( const glimpParms_t parms, int &x, int &y, int &w, int &h ) {
	//
	// compute width and height
	//
	if ( parms.fullScreen != 0 ) {
		if ( /*parms.fullScreen == -1*/1 ) {
			// borderless window at specific location, as for spanning
			// multiple monitor outputs
			x = parms.x;
			y = parms.y;
			w = parms.width;
			h = parms.height;
		} else {
			// get the current monitor position and size on the desktop
			if ( !GetDisplayCoordinates( parms.fullScreen - 1, x, y, w, h ) ) {
				return false;
			}
		}
	} else {
		x = parms.x;
		y = parms.y;
		w = parms.width;
		h = parms.height;

		if ( x == 0 && y == 0 ) {
			// cvars place window at 0,0 by default
			x = SDL_WINDOWPOS_UNDEFINED;
			y = SDL_WINDOWPOS_UNDEFINED;
			return true;
		}
	}

	return true;
}

/*
====================
EventFilter
====================
*/
static int EventFilter( void *data, SDL_Event *event ) {
	if ( event->type == SDL_WINDOWEVENT ) {
		if ( event->window.windowID == SDL_GetWindowID( sdl.window ) ) {
			if ( event->window.event == SDL_WINDOWEVENT_RESIZED ) {
				if ( !R_IsInitialized() || renderSystem->GetWidth() <= 0 || renderSystem->GetHeight() <= 0 ) {
					return 0;
				}

				// restrict to a standard aspect ratio
				int width = event->window.data1;
				int height = event->window.data2;

				// Clamp to a minimum size
				if ( width < SCREEN_WIDTH / 4 ) {
					width = SCREEN_WIDTH / 4;
				}
				if ( height < SCREEN_HEIGHT / 4 ) {
					height = SCREEN_HEIGHT / 4;
				}

				const int minWidth = height * 4 / 3;
				const int maxHeight = width * 3 / 4;

				const int maxWidth = height * 16 / 9;
				const int minHeight = width * 9 / 16;

				SDL_SetWindowMaximumSize( sdl.window, maxWidth, maxHeight );
				SDL_SetWindowMinimumSize( sdl.window, minWidth, minHeight );
			}
		}
	}
	return 0;
}

/*
====================
GLimp_SetScreenParms

Sets up the screen based on passed parms.. 
====================
*/
bool GLimp_SetScreenParms( glimpParms_t parms ) {
	int x, y, w, h;
	if ( !GLimp_GetWindowDimensions( parms, x, y, w, h ) ) {
		return false;
	}

	if ( parms.fullScreen > 0 ) {
		SDL_SetWindowSize( sdl.window, w, h );
		SDL_SetWindowPosition( sdl.window, x, y );

		SDL_SetWindowFullscreen( sdl.window, SDL_WINDOW_FULLSCREEN );
	} else {
		SDL_SetWindowFullscreen( sdl.window, 0 );

		SDL_SetWindowSize( sdl.window, w, h );
		SDL_SetWindowPosition( sdl.window, x, y );
		SDL_SetWindowBordered( sdl.window, ( parms.fullScreen == 0 ? SDL_TRUE : SDL_FALSE ) );

#ifdef ID_WIN
		SDL_SysWMinfo info;
		SDL_VERSION( &info.version );
		if( SDL_GetWindowWMInfo( sdl.window, &info ) ) {
			SetWindowLongPtr( info.info.win.window, GWL_STYLE, WINDOW_STYLE );
			SetClassLongPtr( info.info.win.window, GCLP_HBRBACKGROUND, COLOR_GRAYTEXT );
		}
#endif
	}

	sdl.cdsFullscreen = ( parms.fullScreen > 0 ? parms.fullScreen : 0 );

	glConfig.isFullscreen = parms.fullScreen;
	glConfig.pixelAspect = 1.0f;	// FIXME: some monitor modes may be distorted

	glConfig.isFullscreen = parms.fullScreen;
	glConfig.nativeScreenWidth = parms.width;
	glConfig.nativeScreenHeight = parms.height;

	return true;
}

/*
====================
GLimp_CreateWindow

Responsible for creating the SDL window.
If fullscreen, it won't have a border
====================
*/
static bool GLimp_CreateWindow( glimpParms_t parms ) {
	unsigned int flags = SDL_WINDOW_HIDDEN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE;

	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, ( ( parms.multiSamples > 1 ) ? 1 : 0 ) );
	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, parms.multiSamples );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_STEREO, parms.stereo );

	int useOpenGL32 = r_useOpenGL32.GetInteger();
	const int glMajorVersion = ( useOpenGL32 != 0 ) ? 3 : 2;
	const int glMinorVersion = ( useOpenGL32 != 0 ) ? 2 : 0;
	const int glDebugFlag = r_debugContext.GetBool() ? SDL_GL_CONTEXT_DEBUG_FLAG : 0;
	const int glProfile = ( useOpenGL32 == 1 ) ? SDL_GL_CONTEXT_PROFILE_COMPATIBILITY : ( ( useOpenGL32 == 2 ) ? SDL_GL_CONTEXT_PROFILE_CORE : 0 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, glMajorVersion );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, glMinorVersion );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, glDebugFlag );
	if ( useOpenGL32 != 0 ) {
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, glProfile );
	}

	sdl.window = SDL_CreateWindow( GAME_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, flags);
	
	if ( !sdl.window ) {
		common->Printf( "^3GLimp_CreateWindow() - Couldn't create window^0\n" );
		return false;
	}

	GLimp_SetScreenParms( parms );
	SDL_ShowWindow( sdl.window );

	int x, y, w, h;
	SDL_GetWindowPosition( sdl.window, &x, &y );
	SDL_GetWindowSize( sdl.window, &w, &h );
	common->Printf( "...created window @ %d,%d (%dx%d)\n", x, y, w, h );

	SDL_AddEventWatch( EventFilter, NULL );

	common->Printf( "Initializing OpenGL driver\n" );

	//
	// startup the OpenGL subsystem by creating a context and making it current
	//
	common->Printf( "...creating GL context: " );
	sdl.glContext = SDL_GL_CreateContext( sdl.window );
	if ( !sdl.glContext ) {
		common->Printf( "^3failed^0\n" );
		return false;
	}
	common->Printf( "succeeded\n" );

	common->Printf( "...making context current: " );
	if ( SDL_GL_MakeCurrent(sdl.window, sdl.glContext) != 0 ) {
		SDL_GL_DeleteContext( sdl.glContext );
		sdl.glContext = NULL;
		common->Printf( "^3failed^0\n" );
		return false;
	}
	common->Printf( "succeeded\n" );

	// Check to see if we can get a stereo pixel format, even if we aren't going to use it,
	// so the menu option can be 
	qglGetBooleanv( GL_STEREO, (GLboolean *) & glConfig.stereoPixelFormatAvailable );

	SDL_RaiseWindow( sdl.window );

	glConfig.isFullscreen = parms.fullScreen;

	return true;
}

/*
====================
GLimp_Init

This is the platform specific OpenGL initialization function.  It
is responsible for loading OpenGL, initializing it,
creating a window of the appropriate size, doing
fullscreen manipulations, etc.  Its overall responsibility is
to make sure that a functional OpenGL subsystem is operating
when it returns to the ref.

If there is any failure, the renderer will revert back to safe
parameters and try again.
====================
*/
bool GLimp_Init( glimpParms_t parms ) {
	SDL_DisplayMode desktopDisplayMode;

	cmdSystem->AddCommand( "testSwapBuffers", GLimp_TestSwapBuffers, CMD_FL_SYSTEM, "Times swapbuffer options" );

	common->Printf( "Initializing OpenGL subsystem with multisamples:%i stereo:%i fullscreen:%i\n", 
		parms.multiSamples, parms.stereo, parms.fullScreen );

	// check our desktop attributes
	SDL_GetDesktopDisplayMode( 0, &desktopDisplayMode );
	sdl.desktopBitsPixel = SDL_BITSPERPIXEL( desktopDisplayMode.format );
	sdl.desktopWidth = desktopDisplayMode.w;
	sdl.desktopHeight = desktopDisplayMode.h;

	// we can't run in a window unless it is 32 bpp
	// SDL sometimes reports 32 bpp as 24 bpp so we check for that instead
	if ( sdl.desktopBitsPixel < 24 && parms.fullScreen <= 0 ) {
		common->Printf("^3Windowed mode requires 32 bit desktop depth^0\n");
		return false;
	}

	// try to create a window with the correct pixel format
	// and init the renderer context
	if ( !GLimp_CreateWindow( parms ) ) {
		GLimp_Shutdown();
		return false;
	}

	sdl.cdsFullscreen = ( parms.fullScreen > 0 ? parms.fullScreen : 0 );

	glConfig.isFullscreen = parms.fullScreen;
	glConfig.isStereoPixelFormat = parms.stereo;
	glConfig.nativeScreenWidth = parms.width;
	glConfig.nativeScreenHeight = parms.height;
	glConfig.multisamples = parms.multiSamples;

	glConfig.pixelAspect = 1.0f;	// FIXME: some monitor modes may be distorted
									// should side-by-side stereo modes be consider aspect 0.5?

	glConfig.physicalScreenWidthInCentimeters = 100.0f;

	return true;
}

/*
====================
GLimp_Shutdown

This routine does all OS specific shutdown procedures for the OpenGL
subsystem.
====================
*/
void GLimp_Shutdown() {
	common->Printf( "Shutting down OpenGL subsystem\n" );

	SDL_GL_DeleteContext( sdl.glContext );
	SDL_DestroyWindow( sdl.window );

	// reset display settings
	if ( sdl.cdsFullscreen ) {
		sdl.cdsFullscreen = 0;
	}
}

/*
====================
GLimp_SwapBuffers
====================
*/
void GLimp_SwapBuffers() {
	static bool swapControlTearAvailable = true;

	if ( r_swapInterval.IsModified() ) {
		r_swapInterval.ClearModified();

		for ( int i = 0 ; i < 2 ; i++ ) {
			int interval = 0;
			if ( r_swapInterval.GetInteger() == 1 ) {
				interval = ( swapControlTearAvailable ) ? -1 : 1;
			} else if ( r_swapInterval.GetInteger() == 2 ) {
				interval = 1;
			}

			if ( SDL_GL_SetSwapInterval( interval ) == 0 ) {
				// success
				break;
			} else {
				if ( interval == -1 ) {
					// EXT_swap_control_tear not supported
					swapControlTearAvailable = false;
				} else {
					// EXT_swap_control not supported
					break;
				}
			}
		}
	}

	SDL_GL_SwapWindow( sdl.window );
}

/*
====================
GLimp_ActivateContext
====================
*/
void GLimp_ActivateContext() {
	SDL_GL_MakeCurrent( sdl.window, sdl.glContext );
}

/*
====================
GLimp_DeactivateContext
====================
*/
void GLimp_DeactivateContext() {
	qglFinish();
	SDL_GL_MakeCurrent( sdl.window, NULL );
}

/*
====================
GLimp_ExtensionPointer

Returns a function pointer for an OpenGL extension entry point
====================
*/
GLExtension_t GLimp_ExtensionPointer( const char *name ) {
	void	(*proc)();

	proc = (GLExtension_t)SDL_GL_GetProcAddress( name );

	if ( !proc ) {
		common->Printf( "Couldn't find proc address for: %s\n", name );
	}

	return proc;
}

/*
====================
GLimp_EnableLogging
====================
*/
void GLimp_EnableLogging( bool enable ) {

}

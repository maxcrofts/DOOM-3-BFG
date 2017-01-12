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
/*
** SDL_GLIMP.CPP
**
** This file contains ALL SDL specific stuff having to do with the
** OpenGL refresh.  When a port is being made the following functions
** must be implemented by the port:
**
** GLimp_SwapBuffers
** GLimp_Init
** GLimp_Shutdown
** GLimp_SetGamma
*/
#pragma hdrstop
#include "../../idlib/precompiled.h"

#include "sdl_local.h"
#include "../win32/rc/doom_resource.h"
#include "../../renderer/tr_local.h"


idCVar r_useOpenGL32( "r_useOpenGL32", "1", CVAR_INTEGER, "0 = OpenGL 2.0, 1 = OpenGL 3.2 compatibility profile, 2 = OpenGL 3.2 core profile", 0, 2 );


/*
========================
GLimp_TestSwapBuffers
========================
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
			SDL_GL_SwapWindow( win32.window );
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
========================
GLimp_SetGamma

The renderer calls this when the user adjusts r_gamma or r_brightness
========================
*/
void GLimp_SetGamma( unsigned short red[256], unsigned short green[256], unsigned short blue[256] ) {
	if ( SDL_SetWindowGammaRamp( win32.window, red, green, blue ) == -1 ) {
		common->Printf( "WARNING: SDL_SetWindowGammaRamp failed.\n" );
	}
}


/*
========================
GetDisplayCoordinates
========================
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
	common->Printf( "      dmPosition.x: %i\n", r.x );
	common->Printf( "      dmPosition.y: %i\n", r.y );
	common->Printf( "      dmPelsWidth : %i\n", r.w );
	common->Printf( "      dmPelsHeight: %i\n", r.h );

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
		common->Printf( "      format              : %i\n", SDL_BITSPERPIXEL( displayMode.format ) );
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
			common->Printf( "      format              : %i\n", SDL_BITSPERPIXEL( displayMode.format ) );
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
				common->Printf( "      format              : %i\n", SDL_BITSPERPIXEL( displayMode.format ) );
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
=======================
GLimp_CreateWindow

Responsible for creating the SDL window.
If fullscreen, it won't have a border
=======================
*/
static bool GLimp_CreateWindow( glimpParms_t parms ) {
	int				x, y, w, h;
	if ( !GLimp_GetWindowDimensions( parms, x, y, w, h ) ) {
		return false;
	}

	Uint32 flags = SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE;
	if ( parms.fullScreen == -1 ) {
		flags |= SDL_WINDOW_BORDERLESS;
	} else if ( parms.fullScreen == 1 ) {
		flags |= SDL_WINDOW_FULLSCREEN;
	}

	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, ( ( parms.multiSamples > 1 ) ? 1 : 0 ) );
	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, parms.multiSamples );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_STEREO, ( parms.stereo ? TRUE : FALSE ) );

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

	win32.window = SDL_CreateWindow(
		GAME_NAME,
		x, y, w, h,
		flags);
	
	if ( !win32.window ) {
		common->Printf( "^3GLimp_CreateWindow() - Couldn't create window^0\n" );
		return false;
	}

	SDL_GetWindowPosition( win32.window, &x, &y );
	common->Printf( "...created window @ %d,%d (%dx%d)\n", x, y, w, h );

#if 0
	// Check to see if we can get a stereo pixel format, even if we aren't going to use it,
	// so the menu option can be 
	if ( GLW_ChoosePixelFormat( win32.hDC, parms.multiSamples, true ) != -1 ) {
		glConfig.stereoPixelFormatAvailable = true;
	} else {
		glConfig.stereoPixelFormatAvailable = false;
	}
#endif

	glConfig.stereoPixelFormatAvailable = false;

	common->Printf( "Initializing OpenGL driver\n" );

	//
	// startup the OpenGL subsystem by creating a context and making it current
	//
	common->Printf( "...creating GL context: " );
	win32.glContext = SDL_GL_CreateContext( win32.window );
	if ( !win32.glContext ) {
		common->Printf( "^3failed^0\n" );
		return false;
	}
	common->Printf( "succeeded\n" );

	common->Printf( "...making context current: " );
	if ( SDL_GL_MakeCurrent(win32.window, win32.glContext) != 0 ) {
		SDL_GL_DeleteContext( win32.glContext );
		win32.glContext = NULL;
		common->Printf( "^3failed^0\n" );
		return false;
	}
	common->Printf( "succeeded\n" );

	SDL_RaiseWindow( win32.window );

	glConfig.isFullscreen = parms.fullScreen;

	return true;
}

/*
===================
GLimp_Init

This is the platform specific OpenGL initialization function.  It
is responsible for loading OpenGL, initializing it,
creating a window of the appropriate size, doing
fullscreen manipulations, etc.  Its overall responsibility is
to make sure that a functional OpenGL subsystem is operating
when it returns to the ref.

If there is any failure, the renderer will revert back to safe
parameters and try again.
===================
*/
bool GLimp_Init( glimpParms_t parms ) {
	SDL_DisplayMode desktopDisplayMode;

	cmdSystem->AddCommand( "testSwapBuffers", GLimp_TestSwapBuffers, CMD_FL_SYSTEM, "Times swapbuffer options" );

	common->Printf( "Initializing OpenGL subsystem with multisamples:%i stereo:%i fullscreen:%i\n", 
		parms.multiSamples, parms.stereo, parms.fullScreen );

	// check our desktop attributes
	SDL_GetDesktopDisplayMode( 0, &desktopDisplayMode );
	win32.desktopBitsPixel = SDL_BITSPERPIXEL( desktopDisplayMode.format );
	win32.desktopWidth = desktopDisplayMode.w;
	win32.desktopHeight = desktopDisplayMode.h;

	// we can't run in a window unless it is 32 bpp
	// SDL sometimes reports 32 bpp as 24 bpp so we check for that instead
	if ( win32.desktopBitsPixel < 24 && parms.fullScreen <= 0 ) {
		common->Printf("^3Windowed mode requires 32 bit desktop depth^0\n");
		return false;
	}

	// try to create a window with the correct pixel format
	// and init the renderer context
	if ( !GLimp_CreateWindow( parms ) ) {
		GLimp_Shutdown();
		return false;
	}

	win32.cdsFullscreen = ( parms.fullScreen > 0 ? parms.fullScreen : 0 );

	glConfig.isFullscreen = parms.fullScreen;
	glConfig.isStereoPixelFormat = parms.stereo;
	glConfig.nativeScreenWidth = parms.width;
	glConfig.nativeScreenHeight = parms.height;
	glConfig.multisamples = parms.multiSamples;

	glConfig.pixelAspect = 1.0f;	// FIXME: some monitor modes may be distorted
									// should side-by-side stereo modes be consider aspect 0.5?

	glConfig.physicalScreenWidthInCentimeters = 100.0f;


	// check logging
	GLimp_EnableLogging( ( r_logFile.GetInteger() != 0 ) );

	return true;
}

/*
===================
GLimp_SetScreenParms

Sets up the screen based on passed parms.. 
===================
*/
bool GLimp_SetScreenParms( glimpParms_t parms ) {
	int x, y, w, h;
	if ( !GLimp_GetWindowDimensions( parms, x, y, w, h ) ) {
		return false;
	}

	if ( parms.fullScreen > 0 ) {
		SDL_SetWindowSize( win32.window, w, h );
		SDL_SetWindowPosition( win32.window, x, y );
		SDL_SetWindowFullscreen( win32.window, SDL_WINDOW_FULLSCREEN );
	} else {
		SDL_SetWindowFullscreen( win32.window, 0 );
		SDL_SetWindowSize( win32.window, w, h );
		SDL_SetWindowPosition( win32.window, x, y );
	}

	win32.cdsFullscreen = ( parms.fullScreen > 0 ? parms.fullScreen : 0 );

	glConfig.isFullscreen = parms.fullScreen;
	glConfig.pixelAspect = 1.0f;	// FIXME: some monitor modes may be distorted

	glConfig.isFullscreen = parms.fullScreen;
	glConfig.nativeScreenWidth = parms.width;
	glConfig.nativeScreenHeight = parms.height;

	return true;
}

/*
===================
GLimp_Shutdown

This routine does all OS specific shutdown procedures for the OpenGL
subsystem.
===================
*/
void GLimp_Shutdown() {
	common->Printf( "Shutting down OpenGL subsystem\n" );

//	SDL_QuitSubSystem( SDL_INIT_VIDEO );
	SDL_GL_DeleteContext( win32.glContext );
	SDL_DestroyWindow( win32.window );

	// reset display settings
	if ( win32.cdsFullscreen ) {
		win32.cdsFullscreen = 0;
	}

	// close the thread so the handle doesn't dangle
	if ( win32.renderThreadHandle ) {
		common->Printf( "...closing smp thread\n" );
		CloseHandle( win32.renderThreadHandle );
		win32.renderThreadHandle = NULL;
	}
}

/*
=====================
GLimp_SwapBuffers
=====================
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

	SDL_GL_SwapWindow( win32.window );
}

/*
===========================================================

SMP acceleration

===========================================================
*/

/*
===================
GLimp_ActivateContext
===================
*/
void GLimp_ActivateContext() {
	SDL_GL_MakeCurrent( win32.window, win32.glContext );
}

/*
===================
GLimp_DeactivateContext
===================
*/
void GLimp_DeactivateContext() {
	qglFinish();
	SDL_GL_MakeCurrent( win32.window, NULL );
}


//#define	DEBUG_PRINTS

/*
===================
GLimp_BackEndSleep
===================
*/
void *GLimp_BackEndSleep() {
	void	*data;

#ifdef DEBUG_PRINTS
OutputDebugString( "-->GLimp_BackEndSleep\n" );
#endif
	ResetEvent( win32.renderActiveEvent );

	// after this, the front end can exit GLimp_FrontEndSleep
	SetEvent( win32.renderCompletedEvent );

	WaitForSingleObject( win32.renderCommandsEvent, INFINITE );

	ResetEvent( win32.renderCompletedEvent );
	ResetEvent( win32.renderCommandsEvent );

	data = win32.smpData;

	// after this, the main thread can exit GLimp_WakeRenderer
	SetEvent( win32.renderActiveEvent );

#ifdef DEBUG_PRINTS
OutputDebugString( "<--GLimp_BackEndSleep\n" );
#endif
	return data;
}

/*
===================
GLimp_FrontEndSleep
===================
*/
void GLimp_FrontEndSleep() {
#ifdef DEBUG_PRINTS
OutputDebugString( "-->GLimp_FrontEndSleep\n" );
#endif
	WaitForSingleObject( win32.renderCompletedEvent, INFINITE );

#ifdef DEBUG_PRINTS
OutputDebugString( "<--GLimp_FrontEndSleep\n" );
#endif
}

volatile bool	renderThreadActive;

/*
===================
GLimp_WakeBackEnd
===================
*/
void GLimp_WakeBackEnd( void *data ) {
	int		r;

#ifdef DEBUG_PRINTS
OutputDebugString( "-->GLimp_WakeBackEnd\n" );
#endif
	win32.smpData = data;

	if ( renderThreadActive ) {
		common->FatalError( "GLimp_WakeBackEnd: already active" );
	}

	r = WaitForSingleObject( win32.renderActiveEvent, 0 );
	if ( r == WAIT_OBJECT_0 ) {
		common->FatalError( "GLimp_WakeBackEnd: already signaled" );
	}

	r = WaitForSingleObject( win32.renderCommandsEvent, 0 );
	if ( r == WAIT_OBJECT_0 ) {
		common->FatalError( "GLimp_WakeBackEnd: commands already signaled" );
	}

	// after this, the renderer can continue through GLimp_RendererSleep
	SetEvent( win32.renderCommandsEvent );

	r = WaitForSingleObject( win32.renderActiveEvent, 5000 );

	if ( r == WAIT_TIMEOUT ) {
		common->FatalError( "GLimp_WakeBackEnd: WAIT_TIMEOUT" );
	}

#ifdef DEBUG_PRINTS
OutputDebugString( "<--GLimp_WakeBackEnd\n" );
#endif
}

/*
===================
GLimp_ExtensionPointer

Returns a function pointer for an OpenGL extension entry point
===================
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
==================
GLimp_EnableLogging
==================
*/
void GLimp_EnableLogging( bool enable ) {

}

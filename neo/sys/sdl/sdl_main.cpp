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

#include "../sys_local.h"
#include "sdl_local.h"
#include "../../renderer/tr_local.h"

idCVar sys_arch( "sys_arch", "", CVAR_SYSTEM | CVAR_INIT, "" );
idCVar sys_cpustring( "sys_cpustring", "detect", CVAR_SYSTEM | CVAR_INIT, "" );
idCVar in_mouse( "in_mouse", "1", CVAR_SYSTEM | CVAR_BOOL, "enable mouse input" );
idCVar sys_username( "sys_username", "", CVAR_SYSTEM | CVAR_INIT, "system user name" );
idCVar sys_outputEditString( "sys_outputEditString", "1", CVAR_SYSTEM | CVAR_BOOL, "" );
idCVar sys_viewlog( "sys_viewlog", "0", CVAR_SYSTEM | CVAR_INTEGER, "" );
idCVar sys_allowMultipleInstances( "sys_allowMultipleInstances", "0", CVAR_SYSTEM | CVAR_BOOL, "allow multiple instances running concurrently" );

SDLVars_t sdl;

static char		sys_exepath[MAX_OSPATH];
static char		sys_cmdline[MAX_STRING_CHARS];

static sysMemoryStats_t exeLaunchMemoryStats;

/*
========================
Sys_GetCmdLine
========================
*/
const char * Sys_GetCmdLine() {
	return sys_cmdline;
}

/*
==============
Sys_Quit
==============
*/
void Sys_Quit() {
#ifdef ID_WIN
	timeEndPeriod( 1 );
#endif
	Sys_ShutdownInput();
	Sys_DestroyConsole();
	SDL_Quit();
	_exit( 0 );
}

/*
==============
Sys_Sleep
==============
*/
void Sys_Sleep( int msec ) {
	SDL_Delay( msec );
}

/*
==============
Sys_ShowWindow
==============
*/
void Sys_ShowWindow( bool show ) {
	if ( show ) {
		SDL_ShowWindow( sdl.window );
	} else {
		SDL_HideWindow( sdl.window );
	}
}

/*
==============
Sys_IsWindowVisible
==============
*/
bool Sys_IsWindowVisible() {
	return ( SDL_GetWindowFlags( sdl.window ) & SDL_WINDOW_SHOWN ? true : false );
}

/*
==============
Sys_DefaultBasePath
==============
*/
const char *Sys_DefaultBasePath() {
	static char cwd[MAX_OSPATH];

	char *basePath = SDL_GetBasePath();
	idStr::Copynz( cwd, basePath, MAX_OSPATH );
	SDL_free( basePath );

	return cwd;
}

/*
==============
Sys_EXEPath
==============
*/
const char *Sys_EXEPath() {
	return sys_exepath;
}

/*
================
Sys_GetClipboardData
================
*/
char *Sys_GetClipboardData() {
	char *data = NULL;
	char *cliptext;

	if ( ( cliptext = SDL_GetClipboardText() ) != NULL ) {
		if ( cliptext[0] != '\0' ) {
			int bufsize = strlen( cliptext ) + 1;

			data = (char *)Mem_Alloc( strlen( cliptext ) + 1, TAG_CRAP );
			idStr::Copynz( data, cliptext, bufsize );
				
			strtok( data, "\n\r\b" );
		}
		SDL_free( cliptext );
	}
	return data;
}

/*
================
Sys_SetClipboardData
================
*/
void Sys_SetClipboardData( const char *string ) {
	SDL_SetClipboardText( string );
}


/*
========================================================================

DLL Loading

========================================================================
*/

/*
=====================
Sys_DLL_Load
=====================
*/
void *Sys_DLL_Load( const char *dllName ) {
	void *libHandle = SDL_LoadObject( dllName );
	return libHandle;
}

/*
=====================
Sys_DLL_GetProcAddress
=====================
*/
void *Sys_DLL_GetProcAddress( int dllHandle, const char *procName ) {
	return SDL_LoadFunction( (void *)dllHandle, procName ); 
}

/*
=====================
Sys_DLL_Unload
=====================
*/
void Sys_DLL_Unload( int dllHandle ) {
	if ( !dllHandle ) {
		return;
	}
	SDL_UnloadObject( (void *)dllHandle );
}

/*
========================================================================

EVENT LOOP

========================================================================
*/

#define	MAX_QUED_EVENTS		256
#define	MASK_QUED_EVENTS	( MAX_QUED_EVENTS - 1 )

sysEvent_t	eventQue[MAX_QUED_EVENTS];
int			eventHead = 0;
int			eventTail = 0;

/*
================
Sys_QueEvent

Ptr should either be null, or point to a block of data that can
be freed by the game later.
================
*/
void Sys_QueEvent( sysEventType_t type, int value, int value2, int ptrLength, void *ptr, int inputDeviceNum ) {
	sysEvent_t * ev = &eventQue[ eventHead & MASK_QUED_EVENTS ];

	if ( eventHead - eventTail >= MAX_QUED_EVENTS ) {
		common->Printf("Sys_QueEvent: overflow\n");
		// we are discarding an event, but don't leak memory
		if ( ev->evPtr ) {
			Mem_Free( ev->evPtr );
		}
		eventTail++;
	}

	eventHead++;

	ev->evType = type;
	ev->evValue = value;
	ev->evValue2 = value2;
	ev->evPtrLength = ptrLength;
	ev->evPtr = ptr;
	ev->inputDevice = inputDeviceNum;
}

/*
================
Sys_GenerateEvents
================
*/
void Sys_GenerateEvents() {
	static int entered = false;
	char *s;

	if ( entered ) {
		return;
	}
	entered = true;

	// grab or release the mouse cursor if necessary
	IN_Frame();

	// check for console commands
	s = Sys_ConsoleInput();
	if ( s ) {
		char	*b;
		int		len;

		len = strlen( s ) + 1;
		b = (char *)Mem_Alloc( len, TAG_EVENTS );
		strcpy( b, s );
		Sys_QueEvent( SE_CONSOLE, 0, 0, len, b, 0 );
	}

	entered = false;
}

/*
================
Sys_ClearEvents
================
*/
void Sys_ClearEvents() {
	eventHead = eventTail = 0;
}

/*
================
Sys_GetEvent
================
*/
sysEvent_t Sys_GetEvent() {
	sysEvent_t	ev;

	// return if we have data
	if ( eventHead > eventTail ) {
		eventTail++;
		return eventQue[ ( eventTail - 1 ) & MASK_QUED_EVENTS ];
	}

	// return the empty event 
	memset( &ev, 0, sizeof( ev ) );

	return ev;
}

//================================================================

/*
=================
Sys_In_Restart_f

Restart the input subsystem
=================
*/
void Sys_In_Restart_f( const idCmdArgs &args ) {
	Sys_ShutdownInput();
	Sys_InitInput();
}

/*
================
Sys_Init

The cvar system must already be setup
================
*/
void Sys_Init() {

	cmdSystem->AddCommand( "in_restart", Sys_In_Restart_f, CMD_FL_SYSTEM, "restarts the input system" );

	//
	// User name
	//
	sys_username.SetString( Sys_GetCurrentUser() );

	//
	// CPU type
	//
	if ( !idStr::Icmp( sys_cpustring.GetString(), "detect" ) ) {
		idStr string;

		common->Printf( "%1.0f MHz ", Sys_ClockTicksPerSecond() / 1000000.0f );

		sdl.cpuid = Sys_GetCPUId();

		string.Clear();

		if ( sdl.cpuid & CPUID_AMD ) {
			string += "AMD CPU";
		} else if ( sdl.cpuid & CPUID_INTEL ) {
			string += "Intel CPU";
		} else if ( sdl.cpuid & CPUID_UNSUPPORTED ) {
			string += "unsupported CPU";
		} else {
			string += "generic CPU";
		}

		string += " with ";
		if ( sdl.cpuid & CPUID_MMX ) {
			string += "MMX & ";
		}
		if ( sdl.cpuid & CPUID_3DNOW ) {
			string += "3DNow! & ";
		}
		if ( sdl.cpuid & CPUID_SSE ) {
			string += "SSE & ";
		}
		if ( sdl.cpuid & CPUID_SSE2 ) {
            string += "SSE2 & ";
		}
		if ( sdl.cpuid & CPUID_SSE3 ) {
			string += "SSE3 & ";
		}
		if ( sdl.cpuid & CPUID_HTT ) {
			string += "HTT & ";
		}
		string.StripTrailing( " & " );
		string.StripTrailing( " with " );
		sys_cpustring.SetString( string );
	} else {
		common->Printf( "forcing CPU type to " );
		idLexer src( sys_cpustring.GetString(), idStr::Length( sys_cpustring.GetString() ), "sys_cpustring" );
		idToken token;

		int id = CPUID_NONE;
		while( src.ReadToken( &token ) ) {
			if ( token.Icmp( "generic" ) == 0 ) {
				id |= CPUID_GENERIC;
			} else if ( token.Icmp( "intel" ) == 0 ) {
				id |= CPUID_INTEL;
			} else if ( token.Icmp( "amd" ) == 0 ) {
				id |= CPUID_AMD;
			} else if ( token.Icmp( "mmx" ) == 0 ) {
				id |= CPUID_MMX;
			} else if ( token.Icmp( "3dnow" ) == 0 ) {
				id |= CPUID_3DNOW;
			} else if ( token.Icmp( "sse" ) == 0 ) {
				id |= CPUID_SSE;
			} else if ( token.Icmp( "sse2" ) == 0 ) {
				id |= CPUID_SSE2;
			} else if ( token.Icmp( "sse3" ) == 0 ) {
				id |= CPUID_SSE3;
			} else if ( token.Icmp( "htt" ) == 0 ) {
				id |= CPUID_HTT;
			}
		}
		if ( id == CPUID_NONE ) {
			common->Printf( "WARNING: unknown sys_cpustring '%s'\n", sys_cpustring.GetString() );
			id = CPUID_GENERIC;
		}
		sdl.cpuid = (cpuid_t) id;
	}

	common->Printf( "%s\n", sys_cpustring.GetString() );
	if ( ( sdl.cpuid & CPUID_SSE2 ) == 0 ) {
		common->Error( "SSE2 not supported!" );
	}

	Sys_InitJoystick();
}

/*
================
Sys_Shutdown
================
*/
void Sys_Shutdown() {

}

/*
================
Sys_GetProcessorId
================
*/
cpuid_t Sys_GetProcessorId() {
    return sdl.cpuid;
}

/*
================
Sys_GetProcessorString
================
*/
const char *Sys_GetProcessorString() {
	return sys_cpustring.GetString();
}

//=======================================================================

//#define SET_THREAD_AFFINITY


#define TEST_FPU_EXCEPTIONS	/*	FPU_EXCEPTION_INVALID_OPERATION |		*/	\
							/*	FPU_EXCEPTION_DENORMALIZED_OPERAND |	*/	\
							/*	FPU_EXCEPTION_DIVIDE_BY_ZERO |			*/	\
							/*	FPU_EXCEPTION_NUMERIC_OVERFLOW |		*/	\
							/*	FPU_EXCEPTION_NUMERIC_UNDERFLOW |		*/	\
							/*	FPU_EXCEPTION_INEXACT_RESULT |			*/	\
								0

/*
==================
main
==================
*/
int main( int argc, char *argv[] ) {
	SDL_Init( SDL_INIT_VIDEO );

	SDL_Cursor *curSave = SDL_GetCursor();
	SDL_Cursor *curWait = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_WAIT );
	SDL_SetCursor( curWait );

	Sys_SetPhysicalWorkMemory( 192 << 20, 1024 << 20 );

	idStr::Copynz( sys_exepath, argv[0], sizeof( sys_exepath ) );

	idStr cmdLine;
	for ( int i = 1; i < argc; i++ ) {
		idStr arg;
		arg.Format( "\"%s\"", argv[i] );
		if ( arg.Find( ' ' ) == -1 ) {
			arg.Strip( '"' );
		}
		cmdLine.Append( arg + " " );
	}
	cmdLine.StripTrailingWhitespace();
	idStr::Copynz( sys_cmdline, cmdLine.c_str(), sizeof( sys_cmdline ) );

	// done before Com/Sys_Init since we need this for error output
	Sys_CreateConsole();

#ifdef ID_WIN
	// no abort/retry/fail errors
	SetErrorMode( SEM_FAILCRITICALERRORS );

	// make sure the timer is high precision, otherwise
	// NT gets 18ms resolution
	timeBeginPeriod( 1 );
#endif

#ifdef DEBUG
	// disable the painfully slow MS heap check every 1024 allocs
	_CrtSetDbgFlag( 0 );
#endif

//	Sys_FPU_EnableExceptions( TEST_FPU_EXCEPTIONS );
#ifndef ID_WIN64
	Sys_FPU_SetPrecision( FPU_PRECISION_DOUBLE_EXTENDED );
#endif

	if ( argc > 1 ) {
		common->Init( argc-1, &argv[1], NULL );
	} else {
		common->Init( 0, NULL, NULL );
	}

#if TEST_FPU_EXCEPTIONS != 0
	common->Printf( Sys_FPU_GetState() );
#endif

#if 0
	if ( sdl.win_notaskkeys.GetInteger() ) {
		DisableTaskKeys( TRUE, FALSE, /*( sdl.win_notaskkeys.GetInteger() == 2 )*/ FALSE );
	}
#endif

#ifdef ID_WIN
	// hide or show the early console as necessary
	if ( sys_viewlog.GetInteger() ) {
		Sys_ShowConsole( 1, true );
	} else {
		Sys_ShowConsole( 0, false );
	}
#endif

#ifdef SET_THREAD_AFFINITY 
	// give the main thread an affinity for the first cpu
	SetThreadAffinityMask( GetCurrentThread(), 1 );
#endif

	SDL_SetCursor( curSave );
	SDL_FreeCursor( curWait );

	SDL_RaiseWindow( sdl.window );

    // main game loop
	while ( 1 ) {
#ifdef DEBUG
		Sys_MemFrame();
#endif

		// set exceptions, even if some crappy syscall changes them!
		Sys_FPU_EnableExceptions( TEST_FPU_EXCEPTIONS );

		// run the game
		common->Frame();
	}

	// never gets here
	return 0;
}

/*
==================
Sys_SetFatalError
==================
*/
void Sys_SetFatalError( const char *error ) {
}

/*
================
Sys_SetLanguageFromSystem
================
*/
extern idCVar sys_lang;
void Sys_SetLanguageFromSystem() {
	sys_lang.SetString( Sys_DefaultLanguage() );
}

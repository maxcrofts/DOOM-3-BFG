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

#include <errno.h>
#include <float.h>
#include <fcntl.h>
#include <direct.h>
#include <io.h>
#include <conio.h>
#include <mapi.h>
#include <ShellAPI.h>
#include <Shlobj.h>

#ifndef __MRC__
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "../sys_local.h"
#include "sdl_local.h"
#include "../../renderer/tr_local.h"

idCVar SDLVars_t::sys_arch( "sys_arch", "", CVAR_SYSTEM | CVAR_INIT, "" );
idCVar SDLVars_t::sys_cpustring( "sys_cpustring", "detect", CVAR_SYSTEM | CVAR_INIT, "" );
idCVar SDLVars_t::in_mouse( "in_mouse", "1", CVAR_SYSTEM | CVAR_BOOL, "enable mouse input" );
idCVar SDLVars_t::sys_username( "sys_username", "", CVAR_SYSTEM | CVAR_INIT, "system user name" );

SDLVars_t sdl;

#ifdef ID_PC_WIN
idCVar Win32Vars_t::win_outputEditString( "win_outputEditString", "1", CVAR_SYSTEM | CVAR_BOOL, "" );
idCVar Win32Vars_t::win_viewlog( "win_viewlog", "0", CVAR_SYSTEM | CVAR_INTEGER, "" );
idCVar Win32Vars_t::win_allowMultipleInstances( "win_allowMultipleInstances", "0", CVAR_SYSTEM | CVAR_BOOL, "allow multiple instances running concurrently" );

Win32Vars_t	win32;
#endif

static char		sys_cmdline[MAX_STRING_CHARS];

static sysMemoryStats_t exeLaunchMemoryStats;

static HANDLE hProcessMutex;

/*
================
Sys_GetExeLaunchMemoryStatus
================
*/
void Sys_GetExeLaunchMemoryStatus( sysMemoryStats_t &stats ) {
	stats = exeLaunchMemoryStats;
}

/*
==================
Sys_Sentry
==================
*/
void Sys_Sentry() {
}


#pragma optimize( "", on )

#ifdef DEBUG


static unsigned int debug_total_alloc = 0;
static unsigned int debug_total_alloc_count = 0;
static unsigned int debug_current_alloc = 0;
static unsigned int debug_current_alloc_count = 0;
static unsigned int debug_frame_alloc = 0;
static unsigned int debug_frame_alloc_count = 0;

idCVar sys_showMallocs( "sys_showMallocs", "0", CVAR_SYSTEM, "" );

// _HOOK_ALLOC, _HOOK_REALLOC, _HOOK_FREE

typedef struct CrtMemBlockHeader
{
	struct _CrtMemBlockHeader *pBlockHeaderNext;	// Pointer to the block allocated just before this one:
	struct _CrtMemBlockHeader *pBlockHeaderPrev;	// Pointer to the block allocated just after this one
   char *szFileName;    // File name
   int nLine;           // Line number
   size_t nDataSize;    // Size of user block
   int nBlockUse;       // Type of block
   long lRequest;       // Allocation number
	byte		gap[4];								// Buffer just before (lower than) the user's memory:
} CrtMemBlockHeader;

#include <crtdbg.h>

/*
==================
Sys_AllocHook

	called for every malloc/new/free/delete
==================
*/
int Sys_AllocHook( int nAllocType, void *pvData, size_t nSize, int nBlockUse, long lRequest, const unsigned char * szFileName, int nLine ) 
{
	CrtMemBlockHeader	*pHead;
	byte				*temp;

	if ( nBlockUse == _CRT_BLOCK )
	{
      return( TRUE );
	}

	// get a pointer to memory block header
	temp = ( byte * )pvData;
	temp -= 32;
	pHead = ( CrtMemBlockHeader * )temp;

	switch( nAllocType ) {
		case	_HOOK_ALLOC:
			debug_total_alloc += nSize;
			debug_current_alloc += nSize;
			debug_frame_alloc += nSize;
			debug_total_alloc_count++;
			debug_current_alloc_count++;
			debug_frame_alloc_count++;
			break;

		case	_HOOK_FREE:
			assert( pHead->gap[0] == 0xfd && pHead->gap[1] == 0xfd && pHead->gap[2] == 0xfd && pHead->gap[3] == 0xfd );

			debug_current_alloc -= pHead->nDataSize;
			debug_current_alloc_count--;
			debug_total_alloc_count++;
			debug_frame_alloc_count++;
			break;

		case	_HOOK_REALLOC:
			assert( pHead->gap[0] == 0xfd && pHead->gap[1] == 0xfd && pHead->gap[2] == 0xfd && pHead->gap[3] == 0xfd );

			debug_current_alloc -= pHead->nDataSize;
			debug_total_alloc += nSize;
			debug_current_alloc += nSize;
			debug_frame_alloc += nSize;
			debug_total_alloc_count++;
			debug_current_alloc_count--;
			debug_frame_alloc_count++;
			break;
	}
	return( TRUE );
}

/*
==================
Sys_DebugMemory_f
==================
*/
void Sys_DebugMemory_f() {
  	common->Printf( "Total allocation %8dk in %d blocks\n", debug_total_alloc / 1024, debug_total_alloc_count );
  	common->Printf( "Current allocation %8dk in %d blocks\n", debug_current_alloc / 1024, debug_current_alloc_count );
}

/*
==================
Sys_MemFrame
==================
*/
void Sys_MemFrame() {
	if( sys_showMallocs.GetInteger() ) {
		common->Printf("Frame: %8dk in %5d blocks\n", debug_frame_alloc / 1024, debug_frame_alloc_count );
	}

	debug_frame_alloc = 0;
	debug_frame_alloc_count = 0;
}

#endif

/*
==================
Sys_FlushCacheMemory

On windows, the vertex buffers are write combined, so they
don't need to be flushed from the cache
==================
*/
void Sys_FlushCacheMemory( void *base, int bytes ) {
}

/*
=============
Sys_Error

Show the early console as an error dialog
=============
*/
void Sys_Error( const char *error, ... ) {
	va_list		argptr;
	char		text[4096];
    MSG        msg;

	va_start( argptr, error );
	vsprintf( text, error, argptr );
	va_end( argptr);

	Conbuf_AppendText( text );
	Conbuf_AppendText( "\n" );

	Win_SetErrorText( text );
	Sys_ShowConsole( 1, true );

	timeEndPeriod( 1 );

	Sys_ShutdownInput();

	GLimp_Shutdown();

	extern idCVar com_productionMode;
	if ( com_productionMode.GetInteger() == 0 ) {
		// wait for the user to quit
		while ( 1 ) {
			if ( !GetMessage( &msg, NULL, 0, 0 ) ) {
				common->Quit();
			}
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
	Sys_DestroyConsole();

	exit (1);
}

/*
========================
Sys_Launch
========================
*/
void Sys_Launch( const char * path, idCmdArgs & args,  void * data, unsigned int dataSize ) {

	TCHAR				szPathOrig[_MAX_PATH];
	STARTUPINFO			si;
	PROCESS_INFORMATION	pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);

	strcpy( szPathOrig, va( "\"%s\" %s", Sys_EXEPath(), (const char *)data ) );

	if ( !CreateProcess( NULL, szPathOrig, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) ) {
		idLib::Error( "Could not start process: '%s' ", szPathOrig );
		return;
	}
	cmdSystem->AppendCommandText( "quit\n" );
}

/*
========================
Sys_GetCmdLine
========================
*/
const char * Sys_GetCmdLine() {
	return sys_cmdline;
}

/*
========================
Sys_ReLaunch
========================
*/
void Sys_ReLaunch( void * data, const unsigned int dataSize ) {
	TCHAR				szPathOrig[MAX_PRINT_MSG];
	STARTUPINFO			si;
	PROCESS_INFORMATION	pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);

	strcpy( szPathOrig, va( "\"%s\" %s", Sys_EXEPath(), (const char *)data ) );

	CloseHandle( hProcessMutex );

	if ( !CreateProcess( NULL, szPathOrig, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) ) {
		idLib::Error( "Could not start process: '%s' ", szPathOrig );
		return;
	}
	cmdSystem->AppendCommandText( "quit\n" );
}

/*
==============
Sys_Quit
==============
*/
void Sys_Quit() {
	timeEndPeriod( 1 );
	Sys_ShutdownInput();
	Sys_DestroyConsole();
	SDL_Quit();
	ExitProcess( 0 );
}


/*
==============
Sys_Printf
==============
*/
#define MAXPRINTMSG 4096
void Sys_Printf( const char *fmt, ... ) {
	char		msg[MAXPRINTMSG];

	va_list argptr;
	va_start(argptr, fmt);
	idStr::vsnPrintf( msg, MAXPRINTMSG-1, fmt, argptr );
	va_end(argptr);
	msg[sizeof(msg)-1] = '\0';

	OutputDebugString( msg );

	if ( win32.win_outputEditString.GetBool() && idLib::IsMainThread() ) {
		Conbuf_AppendText( msg );
	}
}

/*
==============
Sys_DebugPrintf
==============
*/
#define MAXPRINTMSG 4096
void Sys_DebugPrintf( const char *fmt, ... ) {
	char msg[MAXPRINTMSG];

	va_list argptr;
	va_start( argptr, fmt );
	idStr::vsnPrintf( msg, MAXPRINTMSG-1, fmt, argptr );
	msg[ sizeof(msg)-1 ] = '\0';
	va_end( argptr );

	OutputDebugString( msg );
}

/*
==============
Sys_DebugVPrintf
==============
*/
void Sys_DebugVPrintf( const char *fmt, va_list arg ) {
	char msg[MAXPRINTMSG];

	idStr::vsnPrintf( msg, MAXPRINTMSG-1, fmt, arg );
	msg[ sizeof(msg)-1 ] = '\0';

	OutputDebugString( msg );
}

/*
==============
Sys_Sleep
==============
*/
void Sys_Sleep( int msec ) {
	Sleep( msec );
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
Sys_Mkdir
==============
*/
void Sys_Mkdir( const char *path ) {
	_mkdir (path);
}

/*
=================
Sys_FileTimeStamp
=================
*/
ID_TIME_T Sys_FileTimeStamp( idFileHandle fp ) {
	FILETIME writeTime;
	GetFileTime( fp, NULL, NULL, &writeTime );

	/*
		FILETIME = number of 100-nanosecond ticks since midnight 
		1 Jan 1601 UTC. time_t = number of 1-second ticks since 
		midnight 1 Jan 1970 UTC. To translate, we subtract a
		FILETIME representation of midnight, 1 Jan 1970 from the
		time in question and divide by the number of 100-ns ticks
		in one second.
	*/

	SYSTEMTIME base_st = {
		1970,   // wYear
		1,      // wMonth
		0,      // wDayOfWeek
		1,      // wDay
		0,      // wHour
		0,      // wMinute
		0,      // wSecond
		0       // wMilliseconds
	};

	FILETIME base_ft;
	SystemTimeToFileTime( &base_st, &base_ft );

	LARGE_INTEGER itime;
	itime.QuadPart = reinterpret_cast<LARGE_INTEGER&>( writeTime ).QuadPart;
	itime.QuadPart -= reinterpret_cast<LARGE_INTEGER&>( base_ft ).QuadPart;
	itime.QuadPart /= 10000000LL;
	return itime.QuadPart;
}

/*
========================
Sys_Rmdir
========================
*/
bool Sys_Rmdir( const char *path ) {
	return _rmdir( path ) == 0;
}

/*
========================
Sys_IsFileWritable
========================
*/
bool Sys_IsFileWritable( const char *path ) {
	struct _stat st;
	if ( _stat( path, &st ) == -1 ) {
		return true;
	}
	return ( st.st_mode & S_IWRITE ) != 0;
}

/*
========================
Sys_IsFolder
========================
*/
sysFolder_t Sys_IsFolder( const char *path ) {
	struct _stat buffer;
	if ( _stat( path, &buffer ) < 0 ) {
		return FOLDER_ERROR;
	}
	return ( buffer.st_mode & _S_IFDIR ) != 0 ? FOLDER_YES : FOLDER_NO;
}

/*
==============
Sys_Cwd
==============
*/
const char *Sys_Cwd() {
	static char cwd[MAX_OSPATH];

	_getcwd( cwd, sizeof( cwd ) - 1 );
	cwd[MAX_OSPATH-1] = 0;

	return cwd;
}

/*
==============
Sys_DefaultBasePath
==============
*/
const char *Sys_DefaultBasePath() {
	return Sys_Cwd();
}

// Vista shit
typedef HRESULT (WINAPI * SHGetKnownFolderPath_t)( const GUID & rfid, DWORD dwFlags, HANDLE hToken, PWSTR *ppszPath );
// NOTE: FOLIDERID_SavedGames is already exported from in shell32.dll in Windows 7.  We can only detect
// the compiler version, but that doesn't doesn't tell us which version of the OS we're linking against.
// This GUID value should never change, so we name it something other than FOLDERID_SavedGames to get
// around this problem.
const GUID FOLDERID_SavedGames_IdTech5 = { 0x4c5c32ff, 0xbb9d, 0x43b0, { 0xb5, 0xb4, 0x2d, 0x72, 0xe5, 0x4e, 0xaa, 0xa4 } };

/*
==============
Sys_DefaultSavePath
==============
*/
const char *Sys_DefaultSavePath() {
	static char savePath[ MAX_PATH ];
	memset( savePath, 0, MAX_PATH );

	HMODULE hShell = LoadLibrary( "shell32.dll" );
	if ( hShell ) {
		SHGetKnownFolderPath_t SHGetKnownFolderPath = (SHGetKnownFolderPath_t)GetProcAddress( hShell, "SHGetKnownFolderPath" );
		if ( SHGetKnownFolderPath ) {
			wchar_t * path;
			if ( SUCCEEDED( SHGetKnownFolderPath( FOLDERID_SavedGames_IdTech5, CSIDL_FLAG_CREATE | CSIDL_FLAG_PER_USER_INIT, 0, &path ) ) ) {
				if ( wcstombs( savePath, path, MAX_PATH ) > MAX_PATH ) {
					savePath[0] = 0;
				}
				CoTaskMemFree( path );
			}
		}
		FreeLibrary( hShell );
	}

	if ( savePath[0] == 0 ) {
		SHGetFolderPath( NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, savePath );
		strcat( savePath, "\\My Games" );
	}

	strcat( savePath, SAVE_PATH );

	return savePath;
}

/*
==============
Sys_EXEPath
==============
*/
const char *Sys_EXEPath() {
	static char exe[ MAX_OSPATH ];
	char *basePath = SDL_GetBasePath();
	idStr::snPrintf( exe, sizeof( exe ) - 1, "%s", basePath );
	SDL_free( basePath );
	return exe;
}

/*
==============
Sys_ListFiles
==============
*/
int Sys_ListFiles( const char *directory, const char *extension, idStrList &list ) {
	idStr		search;
	struct _finddata_t findinfo;
	int			findhandle;
	int			flag;

	if ( !extension) {
		extension = "";
	}

	// passing a slash as extension will find directories
	if ( extension[0] == '/' && extension[1] == 0 ) {
		extension = "";
		flag = 0;
	} else {
		flag = _A_SUBDIR;
	}

	sprintf( search, "%s\\*%s", directory, extension );

	// search
	list.Clear();

	findhandle = _findfirst( search, &findinfo );
	if ( findhandle == -1 ) {
		return -1;
	}

	do {
		if ( flag ^ ( findinfo.attrib & _A_SUBDIR ) ) {
			list.Append( findinfo.name );
		}
	} while ( _findnext( findhandle, &findinfo ) != -1 );

	_findclose( findhandle );

	return list.Num();
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
========================
ExecOutputFn
========================
*/
static void ExecOutputFn( const char * text ) {
	idLib::Printf( text );
}


/*
========================
Sys_Exec

if waitMsec is INFINITE, completely block until the process exits
If waitMsec is -1, don't wait for the process to exit
Other waitMsec values will allow the workFn to be called at those intervals.
========================
*/
bool Sys_Exec(	const char * appPath, const char * workingPath, const char * args, 
	execProcessWorkFunction_t workFn, execOutputFunction_t outputFn, const int waitMS,
	unsigned int & exitCode ) {
		exitCode = 0;
		SECURITY_ATTRIBUTES secAttr;
		secAttr.nLength = sizeof( SECURITY_ATTRIBUTES );
		secAttr.bInheritHandle = TRUE;
		secAttr.lpSecurityDescriptor = NULL;

		HANDLE hStdOutRead;
		HANDLE hStdOutWrite;
		CreatePipe( &hStdOutRead, &hStdOutWrite, &secAttr, 0 );
		SetHandleInformation( hStdOutRead, HANDLE_FLAG_INHERIT, 0 );

		HANDLE hStdInRead;
		HANDLE hStdInWrite;
		CreatePipe( &hStdInRead, &hStdInWrite, &secAttr, 0 );
		SetHandleInformation( hStdInWrite, HANDLE_FLAG_INHERIT, 0 );										

		STARTUPINFO si;
		memset( &si, 0, sizeof( si ) );
		si.cb = sizeof( si );
		si.hStdError = hStdOutWrite;
		si.hStdOutput = hStdOutWrite;
		si.hStdInput = hStdInRead;
		si.wShowWindow = FALSE;
		si.dwFlags |= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

		PROCESS_INFORMATION pi;
		memset ( &pi, 0, sizeof( pi ) );

		if ( outputFn != NULL ) {
			outputFn( va( "^2Executing Process: ^7%s\n^2working path: ^7%s\n^2args: ^7%s\n", appPath, workingPath, args ) );
		} else {
			outputFn = ExecOutputFn;
		}

		// we duplicate args here so we can concatenate the exe name and args into a single command line
		const char * imageName = appPath;
		char * cmdLine = NULL;
		{
			// if we have any args, we need to copy them to a new buffer because CreateProcess modifies
			// the command line buffer.
			if ( args != NULL ) {
				if ( appPath != NULL ) {
					int len = idStr::Length( args ) + idStr::Length( appPath ) + 1 /* for space */ + 1 /* for NULL terminator */ + 2 /* app quotes */;
					cmdLine = (char*)Mem_Alloc( len, TAG_TEMP );
					// note that we're putting quotes around the appPath here because when AAS2.exe gets an app path with spaces
					// in the path "w:/zion/build/win32/Debug with Inlines/AAS2.exe" it gets more than one arg for the app name,
					// which it most certainly should not, so I am assuming this is a side effect of using CreateProcess.
					idStr::snPrintf( cmdLine, len, "\"%s\" %s", appPath, args );
				} else {
					int len = idStr::Length( args ) + 1;
					cmdLine = (char*)Mem_Alloc( len, TAG_TEMP );
					idStr::Copynz( cmdLine, args, len );
				}
				// the image name should always be NULL if we have command line arguments because it is already
				// prefixed to the command line.
				imageName = NULL;
			}
		}

		BOOL result = CreateProcess( imageName, (LPSTR)cmdLine, NULL, NULL, TRUE, 0, NULL, workingPath, &si, &pi );

		if ( result == FALSE ) {
			TCHAR szBuf[1024]; 
			LPVOID lpMsgBuf;
			DWORD dw = GetLastError(); 

			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				dw,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR) &lpMsgBuf,
				0, NULL );

			wsprintf( szBuf, "%d: %s", dw, lpMsgBuf );
			if ( outputFn != NULL ) {
				outputFn( szBuf );
			}
			LocalFree( lpMsgBuf );
			if ( cmdLine != NULL ) {
				Mem_Free( cmdLine );
			}
			return false;
		} else if ( waitMS >= 0 ) {	// if waitMS == -1, don't wait for process to exit
			DWORD ec = 0;
			DWORD wait = 0;
			char buffer[ 4096 ];
			for ( ; ; ) {
				wait = WaitForSingleObject( pi.hProcess, waitMS );
				GetExitCodeProcess( pi.hProcess, &ec );

				DWORD bytesRead = 0;
				DWORD bytesAvail = 0;
				DWORD bytesLeft = 0;
				BOOL ok = PeekNamedPipe( hStdOutRead, NULL, 0, NULL, &bytesAvail, &bytesLeft );
				if ( ok && bytesAvail != 0 ) {
					ok = ReadFile( hStdOutRead, buffer, sizeof( buffer ) - 3, &bytesRead, NULL );
					if ( ok && bytesRead > 0 ) {
						buffer[ bytesRead ] = '\0';
						if ( outputFn != NULL ) {
							int length = 0;
							for ( int i = 0; buffer[i] != '\0'; i++ ) {
								if ( buffer[i] != '\r' ) {
									buffer[length++] = buffer[i];
								}
							}
							buffer[length++] = '\0';
							outputFn( buffer );
						}
					}
				}

				if ( ec != STILL_ACTIVE ) {
					exitCode = ec;
					break;
				}

				if ( workFn != NULL ) {
					if ( !workFn() ) {
						TerminateProcess( pi.hProcess, 0 );
						break;
					}
				}
			}
		}

		// this assumes that windows duplicates the command line string into the created process's
		// environment space.
		if ( cmdLine != NULL ) {
			Mem_Free( cmdLine );
		}

		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
		CloseHandle( hStdOutRead );
		CloseHandle( hStdOutWrite );
		CloseHandle( hStdInRead );
		CloseHandle( hStdInWrite );
		return true;
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
int Sys_DLL_Load( const char *dllName ) {
	void *libHandle = SDL_LoadObject( dllName );
	return (int)libHandle;
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
=============
Sys_PumpEvents

This allows windows to be moved during renderbump
=============
*/
void Sys_PumpEvents() {
    MSG msg;

	// pump the message loop
	while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) {
		if ( !GetMessage( &msg, NULL, 0, 0 ) ) {
			common->Quit();
		}

		// save the msg time, because wndprocs don't have access to the timestamp
		if ( sdl.sysMsgTime && sdl.sysMsgTime > (int)msg.time ) {
			// don't ever let the event times run backwards	
//			common->Printf( "Sys_PumpEvents: sdl.sysMsgTime (%i) > msg.time (%i)\n", sdl.sysMsgTime, msg.time );
		} else {
			sdl.sysMsgTime = msg.time;
		}
 
		TranslateMessage (&msg);
      	DispatchMessage (&msg);
	}
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

	// pump the message loop
	Sys_PumpEvents();

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
Sys_AlreadyRunning

returns true if there is a copy of D3 running already
================
*/
bool Sys_AlreadyRunning() {
#ifndef DEBUG
	if ( !win32.win_allowMultipleInstances.GetBool() ) {
		hProcessMutex = ::CreateMutex( NULL, FALSE, "DOOM3" );
		if ( ::GetLastError() == ERROR_ALREADY_EXISTS || ::GetLastError() == ERROR_ACCESS_DENIED ) {
			return true;
		}
	}
#endif
	return false;
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
	sdl.sys_username.SetString( Sys_GetCurrentUser() );

	//
	// CPU type
	//
	if ( !idStr::Icmp( sdl.sys_cpustring.GetString(), "detect" ) ) {
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
		sdl.sys_cpustring.SetString( string );
	} else {
		common->Printf( "forcing CPU type to " );
		idLexer src( sdl.sys_cpustring.GetString(), idStr::Length( sdl.sys_cpustring.GetString() ), "sys_cpustring" );
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
			common->Printf( "WARNING: unknown sys_cpustring '%s'\n", sdl.sys_cpustring.GetString() );
			id = CPUID_GENERIC;
		}
		sdl.cpuid = (cpuid_t) id;
	}

	common->Printf( "%s\n", sdl.sys_cpustring.GetString() );
	if ( ( sdl.cpuid & CPUID_SSE2 ) == 0 ) {
		common->Error( "SSE2 not supported!" );
	}

	sdl.g_Joystick.Init();
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
	return sdl.sys_cpustring.GetString();
}

//=======================================================================

//#define SET_THREAD_AFFINITY


extern "C" { void _chkstk( int size ); };
void clrstk();

/*
====================
TestChkStk
====================
*/
void TestChkStk() {
	int		buffer[0x1000];

	buffer[0] = 1;
}

/*
====================
HackChkStk
====================
*/
void HackChkStk() {
	DWORD	old;
	VirtualProtect( _chkstk, 6, PAGE_EXECUTE_READWRITE, &old );
	*(byte *)_chkstk = 0xe9;
	*(int *)((int)_chkstk+1) = (int)clrstk - (int)_chkstk - 5;

	TestChkStk();
}

/*
====================
GetExceptionCodeInfo
====================
*/
const char *GetExceptionCodeInfo( UINT code ) {
	switch( code ) {
		case EXCEPTION_ACCESS_VIOLATION: return "The thread tried to read from or write to a virtual address for which it does not have the appropriate access.";
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return "The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking.";
		case EXCEPTION_BREAKPOINT: return "A breakpoint was encountered.";
		case EXCEPTION_DATATYPE_MISALIGNMENT: return "The thread tried to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and so on.";
		case EXCEPTION_FLT_DENORMAL_OPERAND: return "One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value.";
		case EXCEPTION_FLT_DIVIDE_BY_ZERO: return "The thread tried to divide a floating-point value by a floating-point divisor of zero.";
		case EXCEPTION_FLT_INEXACT_RESULT: return "The result of a floating-point operation cannot be represented exactly as a decimal fraction.";
		case EXCEPTION_FLT_INVALID_OPERATION: return "This exception represents any floating-point exception not included in this list.";
		case EXCEPTION_FLT_OVERFLOW: return "The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.";
		case EXCEPTION_FLT_STACK_CHECK: return "The stack overflowed or underflowed as the result of a floating-point operation.";
		case EXCEPTION_FLT_UNDERFLOW: return "The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.";
		case EXCEPTION_ILLEGAL_INSTRUCTION: return "The thread tried to execute an invalid instruction.";
		case EXCEPTION_IN_PAGE_ERROR: return "The thread tried to access a page that was not present, and the system was unable to load the page. For example, this exception might occur if a network connection is lost while running a program over the network.";
		case EXCEPTION_INT_DIVIDE_BY_ZERO: return "The thread tried to divide an integer value by an integer divisor of zero.";
		case EXCEPTION_INT_OVERFLOW: return "The result of an integer operation caused a carry out of the most significant bit of the result.";
		case EXCEPTION_INVALID_DISPOSITION: return "An exception handler returned an invalid disposition to the exception dispatcher. Programmers using a high-level language such as C should never encounter this exception.";
		case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "The thread tried to continue execution after a noncontinuable exception occurred.";
		case EXCEPTION_PRIV_INSTRUCTION: return "The thread tried to execute an instruction whose operation is not allowed in the current machine mode.";
		case EXCEPTION_SINGLE_STEP: return "A trace trap or other single-instruction mechanism signaled that one instruction has been executed.";
		case EXCEPTION_STACK_OVERFLOW: return "The thread used up its stack.";
		default: return "Unknown exception";
	}
}

/*
====================
EmailCrashReport

  emailer originally from Raven/Quake 4
====================
*/
void EmailCrashReport( LPSTR messageText ) {
	static int lastEmailTime = 0;

	if ( Sys_Milliseconds() < lastEmailTime + 10000 ) {
		return;
	}

	lastEmailTime = Sys_Milliseconds();

	HINSTANCE mapi = LoadLibrary( "MAPI32.DLL" ); 
	if( mapi ) {
		LPMAPISENDMAIL	MAPISendMail = ( LPMAPISENDMAIL )GetProcAddress( mapi, "MAPISendMail" );
		if( MAPISendMail ) {
			MapiRecipDesc toProgrammers =
			{
				0,										// ulReserved
					MAPI_TO,							// ulRecipClass
					"DOOM 3 Crash",						// lpszName
					"SMTP:programmers@idsoftware.com",	// lpszAddress
					0,									// ulEIDSize
					0									// lpEntry
			};

			MapiMessage		message = {};
			message.lpszSubject = "DOOM 3 Fatal Error";
			message.lpszNoteText = messageText;
			message.nRecipCount = 1;
			message.lpRecips = &toProgrammers;

			MAPISendMail(
				0,									// LHANDLE lhSession
				0,									// ULONG ulUIParam
				&message,							// lpMapiMessage lpMessage
				MAPI_DIALOG,						// FLAGS flFlags
				0									// ULONG ulReserved
				);
		}
		FreeLibrary( mapi );
	}
}

int Sys_FPU_PrintStateFlags( char *ptr, int ctrl, int stat, int tags, int inof, int inse, int opof, int opse );

/*
====================
_except_handler
====================
*/
EXCEPTION_DISPOSITION __cdecl _except_handler( struct _EXCEPTION_RECORD *ExceptionRecord, void * EstablisherFrame,
												struct _CONTEXT *ContextRecord, void * DispatcherContext ) {

	static char msg[ 8192 ];
	char FPUFlags[2048];

	Sys_FPU_PrintStateFlags( FPUFlags, ContextRecord->FloatSave.ControlWord,
										ContextRecord->FloatSave.StatusWord,
										ContextRecord->FloatSave.TagWord,
										ContextRecord->FloatSave.ErrorOffset,
										ContextRecord->FloatSave.ErrorSelector,
										ContextRecord->FloatSave.DataOffset,
										ContextRecord->FloatSave.DataSelector );


	sprintf( msg, 
		"Please describe what you were doing when DOOM 3 crashed!\n"
		"If this text did not pop into your email client please copy and email it to programmers@idsoftware.com\n"
			"\n"
			"-= FATAL EXCEPTION =-\n"
			"\n"
			"%s\n"
			"\n"
			"0x%x at address 0x%08p\n"
			"\n"
			"%s\n"
			"\n"
			"EAX = 0x%08x EBX = 0x%08x\n"
			"ECX = 0x%08x EDX = 0x%08x\n"
			"ESI = 0x%08x EDI = 0x%08x\n"
			"EIP = 0x%08x ESP = 0x%08x\n"
			"EBP = 0x%08x EFL = 0x%08x\n"
			"\n"
			"CS = 0x%04x\n"
			"SS = 0x%04x\n"
			"DS = 0x%04x\n"
			"ES = 0x%04x\n"
			"FS = 0x%04x\n"
			"GS = 0x%04x\n"
			"\n"
			"%s\n",
			com_version.GetString(),
			ExceptionRecord->ExceptionCode,
			ExceptionRecord->ExceptionAddress,
			GetExceptionCodeInfo( ExceptionRecord->ExceptionCode ),
			ContextRecord->Eax, ContextRecord->Ebx,
			ContextRecord->Ecx, ContextRecord->Edx,
			ContextRecord->Esi, ContextRecord->Edi,
			ContextRecord->Eip, ContextRecord->Esp,
			ContextRecord->Ebp, ContextRecord->EFlags,
			ContextRecord->SegCs,
			ContextRecord->SegSs,
			ContextRecord->SegDs,
			ContextRecord->SegEs,
			ContextRecord->SegFs,
			ContextRecord->SegGs,
			FPUFlags
		);

	EmailCrashReport( msg );
	common->FatalError( msg );

    // Tell the OS to restart the faulting instruction
    return ExceptionContinueExecution;
}

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

	Sys_GetCurrentMemoryStatus( exeLaunchMemoryStats );

#ifdef ID_PC_WIN
	win32.hInstance = GetModuleHandle( NULL );
#endif

	for ( int i = 1; i < argc; i++ ) {
		idStr::Append( sys_cmdline, sizeof( sys_cmdline ), argv[i] );
		idStr::Append( sys_cmdline, sizeof( sys_cmdline ), " " );
	}

	// done before Com/Sys_Init since we need this for error output
	Sys_CreateConsole();

#ifdef ID_PC_WIN
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
	Sys_FPU_SetPrecision( FPU_PRECISION_DOUBLE_EXTENDED );

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

#ifdef ID_PC_WIN
	// hide or show the early console as necessary
	if ( win32.win_viewlog.GetInteger() ) {
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
====================
clrstk

I tried to get the run time to call this at every function entry, but
====================
*/
static int	parmBytes;
__declspec( naked ) void clrstk() {
	// eax = bytes to add to stack
	__asm {
		mov		[parmBytes],eax
        neg     eax                     ; compute new stack pointer in eax
        add     eax,esp
        add     eax,4
        xchg    eax,esp
        mov     eax,dword ptr [eax]		; copy the return address
        push    eax
        
        ; clear to zero
        push	edi
        push	ecx
        mov		edi,esp
        add		edi,12
        mov		ecx,[parmBytes]
		shr		ecx,2
        xor		eax,eax
		cld
        rep	stosd
        pop		ecx
        pop		edi
        
        ret
	}
}

/*
==================
idSysLocal::OpenURL
==================
*/
void idSysLocal::OpenURL( const char *url, bool doexit ) {
	static bool doexit_spamguard = false;
	HWND wnd;

	if (doexit_spamguard) {
		common->DPrintf( "OpenURL: already in an exit sequence, ignoring %s\n", url );
		return;
	}

	common->Printf("Open URL: %s\n", url);

	if ( !ShellExecute( NULL, "open", url, NULL, NULL, SW_RESTORE ) ) {
		common->Error( "Could not open url: '%s' ", url );
		return;
	}

	wnd = GetForegroundWindow();
	if ( wnd ) {
		ShowWindow( wnd, SW_MAXIMIZE );
	}

	if ( doexit ) {
		doexit_spamguard = true;
		cmdSystem->BufferCommandText( CMD_EXEC_APPEND, "quit\n" );
	}
}

/*
==================
idSysLocal::StartProcess
==================
*/
void idSysLocal::StartProcess( const char *exePath, bool doexit ) {
	TCHAR				szPathOrig[_MAX_PATH];
	STARTUPINFO			si;
	PROCESS_INFORMATION	pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);

	strncpy( szPathOrig, exePath, _MAX_PATH );

	if( !CreateProcess( NULL, szPathOrig, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) ) {
        common->Error( "Could not start process: '%s' ", szPathOrig );
	    return;
	}

	if ( doexit ) {
		cmdSystem->BufferCommandText( CMD_EXEC_APPEND, "quit\n" );
	}
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

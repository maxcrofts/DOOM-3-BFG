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

#include <direct.h>
#include <io.h>
#include <Shlobj.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "../sys_local.h"
#include "../sdl/sdl_local.h"
#include "../../renderer/tr_local.h"

static HANDLE hProcessMutex;

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

	if ( sys_outputEditString.GetBool() && idLib::IsMainThread() ) {
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
Sys_Mkdir
==============
*/
void Sys_Mkdir( const char *path ) {
	_mkdir( path );
}

/*
=================
Sys_FileTimeStamp
=================
*/
ID_TIME_T Sys_FileTimeStamp( idFileHandle fp ) {
	struct _stat st;
	_fstat( _fileno( fp ), &st );
	return st.st_mtime;
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
Sys_DefaultSteamPath
==============
*/
const char *Sys_DefaultSteamPath() {
	// detect steam install
	static char basePath[MAX_PATH];
	memset( basePath, 0, MAX_PATH );

	HKEY key;
	DWORD cbData;
	if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Steam App 208200", 0, KEY_READ|KEY_WOW64_64KEY, &key )  == ERROR_SUCCESS ) {
		cbData = MAX_PATH;
		if ( RegQueryValueEx( key, "InstallLocation", NULL, NULL, (LPBYTE)basePath, &cbData ) == ERROR_SUCCESS ) {
			if ( cbData == MAX_PATH ) {
				cbData--;
			}
			basePath[cbData] = '\0';
		} else {
			basePath[0] = 0;
		}
		RegCloseKey( key );
	} else if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Valve\\Steam", 0, KEY_READ|KEY_WOW64_32KEY, &key ) == ERROR_SUCCESS ) {
		cbData = MAX_PATH;
		if ( RegQueryValueEx( key, "InstallPath", NULL, NULL, (LPBYTE)basePath, &cbData ) == ERROR_SUCCESS ) {
			if ( cbData == MAX_PATH ) {
				cbData--;
			}
			basePath[cbData] = '\0';
			idStr::Append( basePath, MAX_PATH, "\\steamapps\\common\\DOOM 3 BFG Edition" );
		} else {
			basePath[0] = 0;
		}
		RegCloseKey( key );
	}

	if ( basePath[0] != 0 && Sys_IsFolder( basePath ) == FOLDER_YES ) {
		return basePath;
	}

	return NULL;
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
Sys_ListFiles
==============
*/
int Sys_ListFiles( const char *directory, const char *extension, idStrList &list ) {
	idStr				search;
	struct _finddata_t	findinfo;
	intptr_t			findhandle;
	int					flag;

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
Sys_AlreadyRunning

returns true if there is a copy of D3 running already
================
*/
bool Sys_AlreadyRunning() {
#ifndef DEBUG
	if ( !sys_allowMultipleInstances.GetBool() ) {
		hProcessMutex = ::CreateMutex( NULL, FALSE, "DOOM3" );
		if ( ::GetLastError() == ERROR_ALREADY_EXISTS || ::GetLastError() == ERROR_ACCESS_DENIED ) {
			return true;
		}
	}
#endif
	return false;
}

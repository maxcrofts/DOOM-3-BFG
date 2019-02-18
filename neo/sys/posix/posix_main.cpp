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

#include <glob.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../sys_local.h"
#include "../sdl/sdl_local.h"
#include "../../renderer/tr_local.h"

/*
========================
Sys_Launch
========================
*/
void Sys_Launch( const char * path, idCmdArgs & args,  void * data, unsigned int dataSize ) {
	Sys_ReLaunch( data, dataSize );
}

/*
========================
Sys_ReLaunch
========================
*/
void Sys_ReLaunch( void * data, const unsigned int dataSize ) {
	static char args[MAX_PRINT_MSG];
	strcpy( args, (const char *)data );
	
	idList<char *> argv;
	argv.Append( (char *)Sys_EXEPath() );
	
	char *saveptr;
	char *ptr = strtok_r( args, " ", &saveptr );
	while ( ptr != NULL ) {
		argv.Append( ptr );
		ptr = strtok_r( NULL, " ", &saveptr );
	}
	
	execv( Sys_EXEPath(), argv.Ptr() );
	idLib::Error( "Could not launch: '\"%s\" %s' ", Sys_EXEPath(), (const char *)data );
}

/*
==============
Sys_DebugVPrintf
==============
*/
void Sys_DebugVPrintf( const char *fmt, va_list arg ) {
	char msg[4096];

	idStr::vsnPrintf( msg, sizeof( msg ), fmt, arg );
	msg[sizeof(msg)-1] = '\0';

	printf( "%s", msg );
}

/*
==============
Sys_Printf
==============
*/
void Sys_Printf( const char *fmt, ... ) {
	va_list argptr;
	va_start( argptr, fmt );
	Sys_DebugVPrintf( fmt, argptr );
	va_end( argptr );
}

/*
==============
Sys_DebugPrintf
==============
*/
void Sys_DebugPrintf( const char *fmt, ... ) {
	va_list argptr;
	va_start( argptr, fmt );
	Sys_DebugVPrintf( fmt, argptr );
	va_end( argptr );
}

/*
=============
Sys_Error
=============
*/
void Sys_Error( const char *error, ... ) {
	va_list argptr;
	va_start( argptr, error );
	Sys_DebugVPrintf( error, argptr );
	va_end( argptr );
	Sys_Printf( "\n" );

	_exit( 1 );
}

/*
==============
Sys_Mkdir
==============
*/
void Sys_Mkdir( const char *path ) {
	mkdir( path, 0777 );
}

/*
=================
Sys_FileTimeStamp
=================
*/
ID_TIME_T Sys_FileTimeStamp( idFileHandle fp ) {
	struct stat st;
	fstat( fileno( fp ), &st );
	return st.st_mtime;
}

/*
========================
Sys_Rmdir
========================
*/
bool Sys_Rmdir( const char *path ) {
	return rmdir( path ) == 0;
}

/*
========================
Sys_IsFileWritable
========================
*/
bool Sys_IsFileWritable( const char *path ) {
	struct stat st;
	if ( stat( path, &st ) == -1 ) {
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
	struct stat st;
	if ( stat( path, &st ) < 0 ) {
		return FOLDER_ERROR;
	}
	return ( st.st_mode & S_IFDIR ) != 0 ? FOLDER_YES : FOLDER_NO;
}

/*
==============
Sys_DefaultSteamPath
==============
*/
const char *Sys_DefaultSteamPath() {
	return NULL;
}

/*
==============
Sys_DefaultSavePath
==============
*/
const char *Sys_DefaultSavePath() {
	idStr savePath( SAVE_PATH );
	savePath.StripPath();
	
	return SDL_GetPrefPath( "", savePath.c_str() );
}

/*
==============
Sys_ListFiles
==============
*/
int Sys_ListFiles( const char *directory, const char *extension, idStrList &list ) {
	idStr search;
	glob_t gl;
	struct stat st;
	mode_t flag;

	if ( !extension ) {
		extension = "";
	}

	// passing a slash as extension will find directories
	if ( extension[0] == '/' && extension[1] == 0 ) {
		extension = "";
		flag = 0;
	} else {
		flag = S_IFDIR;
	}

	sprintf( search, "%s/*%s", directory, extension );

	// search
	list.Clear();

	if ( glob( search, 0, NULL, &gl ) != 0 ) {
		return -1;
	}

	for ( int i = 0; i < gl.gl_pathc; i++ ) {
		if( stat( gl.gl_pathv[i], &st ) == -1 ) {
			continue;
		}
		if ( flag ^ ( st.st_mode & S_IFDIR ) ) {
			idStr file( gl.gl_pathv[i] );
			file.StripPath();
			list.Append( file );
		}
	}

	globfree( &gl );

	return list.Num();
}

/*
================
Sys_AlreadyRunning

returns true if there is a copy of D3 running already
================
*/
bool Sys_AlreadyRunning() {
	return false;
}

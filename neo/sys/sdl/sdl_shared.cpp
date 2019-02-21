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

#include "sdl_local.h"

#ifndef ID_WIN
#include <sys/statvfs.h>
#endif

/*
================
Sys_Milliseconds
================
*/
int Sys_Milliseconds() {
	return SDL_GetTicks();
}

/*
========================
Sys_Microseconds
========================
*/
uint64 Sys_Microseconds() {
	static uint64 ticksPerMicrosecondTimes1024 = 0;

	if ( ticksPerMicrosecondTimes1024 == 0 ) {
		ticksPerMicrosecondTimes1024 = ( (uint64)Sys_ClockTicksPerSecond() << 10 ) / 1000000;
		assert( ticksPerMicrosecondTimes1024 > 0 );
	}

	return ((uint64)( (int64)Sys_GetClockTicks() << 10 )) / ticksPerMicrosecondTimes1024;
}

/*
========================
Sys_GetDriveFreeSpaceInBytes
========================
*/
int64 Sys_GetDriveFreeSpaceInBytes( const char * path ) {
	int64 ret = 1;
	
#ifdef ID_WIN
	DWORDLONG lpFreeBytesAvailable;
	DWORDLONG lpTotalNumberOfBytes;
	DWORDLONG lpTotalNumberOfFreeBytes;
	//FIXME: see why this is failing on some machines
	if ( ::GetDiskFreeSpaceEx( path, (PULARGE_INTEGER)&lpFreeBytesAvailable, (PULARGE_INTEGER)&lpTotalNumberOfBytes, (PULARGE_INTEGER)&lpTotalNumberOfFreeBytes ) ) {
		ret = lpFreeBytesAvailable;
	}
#else
	struct statvfs buffer;
	if ( statvfs( path, &buffer ) == 0 ) {
		ret = buffer.f_bsize * buffer.f_bavail;
	}
#endif
	
	return ret;
}

/*
================
Sys_LockMemory
================
*/
bool Sys_LockMemory( void *ptr, int bytes ) {
#ifdef ID_WIN
	return ( VirtualLock( ptr, (SIZE_T)bytes ) != FALSE );
#else
	return false;
#endif
}

/*
================
Sys_UnlockMemory
================
*/
bool Sys_UnlockMemory( void *ptr, int bytes ) {
#ifdef ID_WIN
	return ( VirtualUnlock( ptr, (SIZE_T)bytes ) != FALSE );
#else
	return false;
#endif
}

/*
================
Sys_SetPhysicalWorkMemory
================
*/
void Sys_SetPhysicalWorkMemory( int minBytes, int maxBytes ) {
#ifdef ID_WIN
	::SetProcessWorkingSetSize( GetCurrentProcess(), minBytes, maxBytes );
#endif
}

/*
================
Sys_GetCurrentUser
================
*/
char *Sys_GetCurrentUser() {
	static char s_userName[1024];
	unsigned int size = sizeof( s_userName );

#ifdef ID_WIN
	if ( !GetUserName( s_userName, &size ) ) {
		strcpy( s_userName, "player" );
	}
#endif

	if ( !s_userName[0] ) {
		strcpy( s_userName, "player" );
	}

	return s_userName;
}

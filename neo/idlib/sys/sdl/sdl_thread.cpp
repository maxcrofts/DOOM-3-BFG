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
#include "../../precompiled.h"

/*
================================================================================================

	Thread

================================================================================================
*/

/*
========================
Sys_CreateThread
========================
*/
uintptr_t Sys_CreateThread( xthread_t function, void *parms, xthreadPriority priority, const char *name, core_t core, int stackSize, bool suspended ) {

	SDL_Thread *thread = SDL_CreateThread( (SDL_ThreadFunction)function, name, parms );
	if ( thread == NULL ) {
		idLib::common->FatalError( "CreateThread error: %i", SDL_GetError() );
		return (uintptr_t)0;
	}

	// we don't set the thread affinity and let the OS deal with scheduling

	return (uintptr_t)thread;
}


/*
========================
Sys_GetCurrentThreadID
========================
*/
uintptr_t Sys_GetCurrentThreadID() {
	return SDL_ThreadID();
}

/*
========================
Sys_WaitForThread
========================
*/
void Sys_WaitForThread( uintptr_t threadHandle ) {
	SDL_WaitThread( (SDL_Thread *)threadHandle, NULL );
}

/*
========================
Sys_DestroyThread
========================
*/
void Sys_DestroyThread( uintptr_t threadHandle ) {
	if ( threadHandle == 0 ) {
		return;
	}
	SDL_DetachThread( (SDL_Thread *)threadHandle );
}

/*
========================
Sys_Yield
========================
*/
void Sys_Yield() {
}

/*
================================================================================================

	Condition Variable

================================================================================================
*/

/*
========================
Sys_CondCreate
========================
*/
void Sys_CondCreate( condHandle_t & handle ) {
	handle = SDL_CreateCond();
}

/*
========================
Sys_CondDestroy
========================
*/
void Sys_CondDestroy( condHandle_t & handle ) {
	SDL_DestroyCond( handle );
}

/*
========================
Sys_CondBroadcast
========================
*/
void Sys_CondBroadcast( condHandle_t & handle ) {
	SDL_CondBroadcast( handle );
}

/*
========================
Sys_CondSignal
========================
*/
void Sys_CondSignal( condHandle_t & handle ) {
	SDL_CondSignal( handle );
}

/*
========================
Sys_CondWait
========================
*/
int Sys_CondWait( condHandle_t & condHandle, mutexHandle_t & mutexHandle, int timeout ) {
	return SDL_CondWaitTimeout( condHandle, mutexHandle, timeout );
}

/*
================================================================================================

	Mutex

================================================================================================
*/

/*
========================
Sys_MutexCreate
========================
*/
void Sys_MutexCreate( mutexHandle_t & handle ) {
	handle = SDL_CreateMutex();
}

/*
========================
Sys_MutexDestroy
========================
*/
void Sys_MutexDestroy( mutexHandle_t & handle ) {
	SDL_DestroyMutex( handle );
}

/*
========================
Sys_MutexLock
========================
*/
bool Sys_MutexLock( mutexHandle_t & handle, bool blocking ) {
	if ( !blocking ) {
		if ( SDL_TryLockMutex( handle ) == 0 ) {
			return false;
		}
	} else {
		SDL_LockMutex( handle );
	}
	return true;
}

/*
========================
Sys_MutexUnlock
========================
*/
void Sys_MutexUnlock( mutexHandle_t & handle ) {
	SDL_UnlockMutex( handle );
}

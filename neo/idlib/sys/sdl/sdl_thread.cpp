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

#include "SDL_hints.h"
#include "SDL_thread.h"
#include "SDL_mutex.h"

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
uintptr_t Sys_CreateThread( xthread_t function, void *parms, const char *name, core_t core, int stackSize, bool suspended ) {
	SDL_SetHint( SDL_HINT_THREAD_STACK_SIZE, va( "%d", stackSize ) );

	SDL_Thread *thread = SDL_CreateThread( (SDL_ThreadFunction)function, name, parms );
	if ( thread == NULL ) {
		idLib::common->FatalError( "CreateThread error: %i", SDL_GetError() );
		return 0;
	}

	// we don't set the thread affinity and let the OS deal with scheduling

	return reinterpret_cast<uintptr_t>( thread );
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
Sys_SetCurrentThreadPriority
========================
*/
void Sys_SetCurrentThreadPriority( xthreadPriority priority ) {
	SDL_SetThreadPriority( static_cast<SDL_ThreadPriority>( priority ) );
}

/*
========================
Sys_WaitForThread
========================
*/
void Sys_WaitForThread( uintptr_t threadHandle ) {
	SDL_WaitThread( reinterpret_cast<SDL_Thread *>( threadHandle ), NULL );
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
	SDL_DetachThread( reinterpret_cast<SDL_Thread *>( threadHandle ) );
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
	SDL_DestroyCond( static_cast<SDL_cond *>( handle ) );
}

/*
========================
Sys_CondBroadcast
========================
*/
void Sys_CondBroadcast( condHandle_t & handle ) {
	SDL_CondBroadcast( static_cast<SDL_cond *>( handle ) );
}

/*
========================
Sys_CondSignal
========================
*/
void Sys_CondSignal( condHandle_t & handle ) {
	SDL_CondSignal( static_cast<SDL_cond *>( handle ) );
}

/*
========================
Sys_CondWait
========================
*/
int Sys_CondWait( condHandle_t & condHandle, mutexHandle_t & mutexHandle ) {
	return Sys_CondWait( condHandle, mutexHandle, SDL_MUTEX_MAXWAIT );
}

/*
========================
Sys_CondWait
========================
*/
int Sys_CondWait( condHandle_t & condHandle, mutexHandle_t & mutexHandle, int timeout ) {
	return SDL_CondWaitTimeout( static_cast<SDL_cond *>( condHandle ), static_cast<SDL_mutex *>( mutexHandle ), timeout );
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
	SDL_DestroyMutex( static_cast<SDL_mutex *>( handle ) );
}

/*
========================
Sys_MutexLock
========================
*/
bool Sys_MutexLock( mutexHandle_t & handle, bool blocking ) {
	if ( !blocking ) {
		if ( SDL_TryLockMutex( static_cast<SDL_mutex *>( handle ) ) == 0 ) {
			return false;
		}
	} else {
		SDL_LockMutex( static_cast<SDL_mutex *>( handle ) );
	}
	return true;
}

/*
========================
Sys_MutexUnlock
========================
*/
void Sys_MutexUnlock( mutexHandle_t & handle ) {
	SDL_UnlockMutex( static_cast<SDL_mutex *>( handle ) );
}

/*
================================================================================================

	Interlocked Integer

================================================================================================
*/

/*
========================
Sys_InterlockedIncrement
========================
*/
interlockedInt_t Sys_InterlockedIncrement( interlockedInt_t & value ) {
	SDL_AtomicIncRef( (SDL_atomic_t *) & value );
	return SDL_AtomicGet( (SDL_atomic_t *) & value );
}

/*
========================
Sys_InterlockedDecrement
========================
*/
interlockedInt_t Sys_InterlockedDecrement( interlockedInt_t & value ) {
	SDL_AtomicDecRef( (SDL_atomic_t *) & value );
	return SDL_AtomicGet( (SDL_atomic_t *) & value );
}

/*
========================
Sys_InterlockedAdd
========================
*/
interlockedInt_t Sys_InterlockedAdd( interlockedInt_t & value, interlockedInt_t i ) {
	return SDL_AtomicAdd( (SDL_atomic_t *) & value, i ) + i;
}

/*
========================
Sys_InterlockedSub
========================
*/
interlockedInt_t Sys_InterlockedSub( interlockedInt_t & value, interlockedInt_t i ) {
	return SDL_AtomicAdd( (SDL_atomic_t *) & value, -i ) - i;
}

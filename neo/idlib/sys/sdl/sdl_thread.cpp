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
Sys_SetCurrentThreadName
========================
*/
void Sys_SetCurrentThreadName( const char * name ) {

}

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

	Signal

================================================================================================
*/

/*
========================
Sys_SignalCreate
========================
*/
void Sys_SignalCreate( signalHandle_t & handle, bool manualReset ) {
	handle = (signalHandle_t) malloc( sizeof( Signal ) );
	handle->manualReset = manualReset;
	handle->signaled = false;
	handle->mutex = SDL_CreateMutex();
	handle->condition = SDL_CreateCond();
}

/*
========================
Sys_SignalDestroy
========================
*/
void Sys_SignalDestroy( signalHandle_t &handle ) {
	SDL_DestroyMutex( handle->mutex );
	SDL_DestroyCond( handle->condition );
	free( handle );
}

/*
========================
Sys_SignalRaise
========================
*/
void Sys_SignalRaise( signalHandle_t & handle ) {
	SDL_LockMutex( handle->mutex );
	if ( handle->manualReset ) {
		handle->signaled = true;
		SDL_UnlockMutex( handle->mutex );
		SDL_CondBroadcast( handle->condition );
	} else {
		handle->signaled = true;
		SDL_UnlockMutex( handle->mutex );
		SDL_CondSignal( handle->condition );
	}
}

/*
========================
Sys_SignalClear
========================
*/
void Sys_SignalClear( signalHandle_t & handle ) {
	SDL_LockMutex( handle->mutex );
	handle->signaled = false;
	SDL_UnlockMutex( handle->mutex );
}

/*
========================
Sys_SignalWait
========================
*/
bool Sys_SignalWait( signalHandle_t & handle, int timeout ) {
	SDL_LockMutex( handle->mutex );
	bool result = true;
	if ( handle->signaled ) {
		if ( !handle->manualReset ) {
			handle->signaled = false;
		}
	} else {
		if ( timeout == 0 ) {
			result = false;
		} else if ( timeout == idSysSignal::WAIT_INFINITE ) {
			while ( !handle->signaled ) {
				SDL_CondWait( handle->condition, handle->mutex );
			}
		} else {
			while ( !handle->signaled ) {
				if ( SDL_CondWaitTimeout( handle->condition, handle->mutex, timeout ) == SDL_MUTEX_TIMEDOUT ) {
					result = false;
					break;
				}
			}
		}
	}
	SDL_UnlockMutex( handle->mutex );
	assert( result || ( timeout != idSysSignal::WAIT_INFINITE && !result ) );
	return result;
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

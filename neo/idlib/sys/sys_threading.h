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
#ifndef __SYS_THREADING_H__
#define __SYS_THREADING_H__

#ifndef __TYPEINFOGEN__

/*
================================================================================================

	Platform specific mutex, condition variable and atomic integer.

================================================================================================
*/

	typedef void *					mutexHandle_t;
	typedef void *					condHandle_t;
	typedef int						interlockedInt_t;

#endif // __TYPEINFOGEN__

/*
================================================================================================

	Platform independent threading functions.

================================================================================================
*/

enum core_t {
	CORE_ANY = -1,
	CORE_0A,
	CORE_0B,
	CORE_1A,
	CORE_1B,
	CORE_2A,
	CORE_2B
};

typedef unsigned int (*xthread_t)( void * );

enum xthreadPriority {
	THREAD_LOW,
	THREAD_NORMAL,
	THREAD_HIGH,
	THREAD_TIME_CRITICAL
};

#define DEFAULT_THREAD_STACK_SIZE		( 256 * 1024 )

// returns a threadHandle
uintptr_t			Sys_CreateThread( xthread_t function, void *parms, const char *name, 
									  core_t core, int stackSize = DEFAULT_THREAD_STACK_SIZE, 
									  bool suspended = false );

uintptr_t			Sys_GetCurrentThreadID();
void				Sys_SetCurrentThreadPriority( xthreadPriority priority );

void				Sys_WaitForThread( uintptr_t threadHandle );
void				Sys_DestroyThread( uintptr_t threadHandle );

void				Sys_CondCreate( condHandle_t & handle );
void				Sys_CondDestroy( condHandle_t & handle );
void				Sys_CondBroadcast( condHandle_t & handle );
void				Sys_CondSignal( condHandle_t & handle );
int					Sys_CondWait( condHandle_t & condHandle, mutexHandle_t & mutexHandle );
int					Sys_CondWait( condHandle_t & condHandle, mutexHandle_t & mutexHandle, int timeout );

void				Sys_MutexCreate( mutexHandle_t & handle );
void				Sys_MutexDestroy( mutexHandle_t & handle );
bool				Sys_MutexLock( mutexHandle_t & handle, bool blocking );
void				Sys_MutexUnlock( mutexHandle_t & handle );

interlockedInt_t	Sys_InterlockedIncrement( interlockedInt_t & value );
interlockedInt_t	Sys_InterlockedDecrement( interlockedInt_t & value );

interlockedInt_t	Sys_InterlockedAdd( interlockedInt_t & value, interlockedInt_t i );
interlockedInt_t	Sys_InterlockedSub( interlockedInt_t & value, interlockedInt_t i );

void				Sys_Yield();

const int MAX_CRITICAL_SECTIONS		= 4;

enum {
	CRITICAL_SECTION_ZERO = 0,
	CRITICAL_SECTION_ONE,
	CRITICAL_SECTION_TWO,
	CRITICAL_SECTION_THREE
};

#endif	// !__SYS_THREADING_H__

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
#include "../precompiled.h"

#include "SDL_atomic.h"

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

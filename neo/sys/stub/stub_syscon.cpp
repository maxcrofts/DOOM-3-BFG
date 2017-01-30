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

#include "../sdl/sdl_local.h"

#define	COMMAND_HISTORY	64

typedef struct {
	char		errorString[80];

	char		consoleText[512], returnedText[512];
	bool		quitOnClose;
	int			windowWidth, windowHeight;

	idEditField	historyEditLines[COMMAND_HISTORY];

	int			nextHistoryLine;// the last line in the history buffer, not masked
	int			historyLine;	// the line being displayed from history buffer
								// will be <= nextHistoryLine

	idEditField	consoleField;

} SysConData;

static SysConData scd;

/*
====================
Sys_CreateConsole
====================
*/
void Sys_CreateConsole() {
}

/*
====================
Sys_DestroyConsole
====================
*/
void Sys_DestroyConsole() {
}

/*
====================
Sys_ShowConsole
====================
*/
void Sys_ShowConsole( int visLevel, bool quitOnClose ) {
	scd.quitOnClose = quitOnClose;
}

/*
====================
Sys_ConsoleInput
====================
*/
char *Sys_ConsoleInput() {	
	if ( scd.consoleText[0] == 0 ) {
		return NULL;
	}
		
	strcpy( scd.returnedText, scd.consoleText );
	scd.consoleText[0] = 0;
	
	return scd.returnedText;
}

/*
====================
Conbuf_AppendText
====================
*/
void Conbuf_AppendText( const char *pMsg ) {
}

/*
====================
Win_SetErrorText
====================
*/
void Win_SetErrorText( const char *buf ) {
	idStr::Copynz( scd.errorString, buf, sizeof( scd.errorString ) );
}

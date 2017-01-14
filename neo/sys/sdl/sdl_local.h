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

#ifndef __SDL_LOCAL_H__
#define __SDL_LOCAL_H__

#include "SDL.h"
#include "sdl_input.h"

#ifdef ID_PC_WIN
#define	WINDOW_STYLE	(WS_OVERLAPPED|WS_BORDER|WS_CAPTION|WS_VISIBLE|WS_THICKFRAME|WS_SYSMENU)
#endif

void	Sys_QueEvent( sysEventType_t type, int value, int value2, int ptrLength, void *ptr, int inputDeviceNum );

void	Sys_CreateConsole();
void	Sys_DestroyConsole();

char	*Sys_ConsoleInput ();
char	*Sys_GetCurrentUser();

void	Win_SetErrorText( const char *text );

cpuid_t	Sys_GetCPUId();

// Input subsystem

void	IN_Init ();
void	IN_Shutdown ();
// add additional non keyboard / non mouse movement on top of the keyboard move cmd

void	IN_DeactivateMouseIfWindowed();
void	IN_DeactivateMouse();
void	IN_ActivateMouse();

void	IN_Frame();

uint64 Sys_Microseconds();

void Conbuf_AppendText( const char *msg );

typedef struct {
	SDL_Window		*window;
	SDL_GLContext	glContext;

	bool			activeApp;			// changed with WM_ACTIVATE messages
	bool			mouseReleased;		// when the game has the console down or is doing a long operation
	bool			movingWindow;		// inhibit mouse grab when dragging the window
	bool			mouseGrabbed;		// current state of grab and hide

	cpuid_t			cpuid;

	// when we get a windows message, we store the time off so keyboard processing
	// can know the exact time of an event (not really needed now that we use async direct input)
	int				sysMsgTime;

	bool			windowClassRegistered;

	int				desktopBitsPixel;
	int				desktopWidth, desktopHeight;

	int				cdsFullscreen;	// 0 = not fullscreen, otherwise monitor number

	idFileHandle	log_fp;

	unsigned short	oldHardwareGamma[3][256];
	// desktop gamma is saved here for restoration at exit

	static idCVar	sys_arch;
	static idCVar	sys_cpustring;
	static idCVar	in_mouse;
	static idCVar	sys_username;
	static idCVar	sdl_timerUpdate;

	idJoystickSDL	g_Joystick;

	void			*renderCommandsEvent;
	void			*renderCompletedEvent;
	void			*renderActiveEvent;
	void			*renderThreadHandle;
	unsigned long	renderThreadId;
	void			(*glimpRenderThread)();
	void			*smpData;
	int				wglErrors;
	// SMP acceleration vars

} SDLVars_t;

extern SDLVars_t	sdl;

#ifdef ID_PC_WIN
typedef struct {
	HINSTANCE		hInstance;
	
	static idCVar	win_outputEditString;
	static idCVar	win_viewlog;
	static idCVar	win_allowMultipleInstances;

} Win32Vars_t;

extern Win32Vars_t	win32;
#endif

#endif /* !__SDL_LOCAL_H__ */

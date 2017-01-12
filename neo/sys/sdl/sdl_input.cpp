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
#include "../sys_session_local.h"

#include "sdl_local.h"
#include "../../renderer/tr_local.h"

#define DINPUT_BUFFERSIZE           256

static inputEvent_t poll_events_keyboard[DINPUT_BUFFERSIZE];
static int poll_keyboard_event_count;
static inputEvent_t poll_events_mouse[MAX_MOUSE_EVENTS];
static int poll_mouse_event_count;

static SDL_GameController *controller = NULL;
SDL_Haptic *haptic;

extern idCVar r_windowX;
extern idCVar r_windowY;
extern idCVar r_windowWidth;
extern idCVar r_windowHeight;


/*
==========
PostKeyboardEvent
==========
*/
bool PostKeyboardEvent( int key, bool state ) {
	if( poll_keyboard_event_count > DINPUT_BUFFERSIZE ) {
		common->DPrintf( "poll_keyboard_event_count exceeded DINPUT_BUFFERSIZE\n" );
		return false;
	}

	poll_events_keyboard[poll_keyboard_event_count].event = key;
	poll_events_keyboard[poll_keyboard_event_count].value = state;
	poll_keyboard_event_count++;

	return true;
}

/*
==========
PostMouseEvent
==========
*/
bool PostMouseEvent( int action, int value ) {
	if( poll_mouse_event_count > MAX_MOUSE_EVENTS ) {
		common->DPrintf( "WARNING: poll_mouse_event_count exceeded MAX_MOUSE_EVENTS\n" );
		return false;
	}

	poll_events_mouse[poll_mouse_event_count].event = action;
	poll_events_mouse[poll_mouse_event_count].value = value;
	poll_mouse_event_count++;

	return true;
}

/*
==========================
IN_ActivateMouse
==========================
*/
void IN_ActivateMouse() {
	if ( !win32.in_mouse.GetBool() || win32.mouseGrabbed ) {
		return;
	}

	win32.mouseGrabbed = true;
	SDL_ShowCursor( SDL_DISABLE );

	// set the cooperativity level.
	SDL_SetRelativeMouseMode( SDL_TRUE );
}

/*
==========================
IN_DeactivateMouse
==========================
*/
void IN_DeactivateMouse() {
	if ( !win32.mouseGrabbed ) {
		return;
	}

	SDL_SetRelativeMouseMode( SDL_FALSE );

	SDL_ShowCursor( SDL_ENABLE );
	win32.mouseGrabbed = false;
}

/*
==========================
IN_DeactivateMouseIfWindowed
==========================
*/
void IN_DeactivateMouseIfWindowed() {
	if ( !win32.cdsFullscreen ) {
		IN_DeactivateMouse();
	}
}

/*
===========
Sys_ShutdownInput
===========
*/
void Sys_ShutdownInput() {
	IN_DeactivateMouse();
}

/*
===========
Sys_InitInput
===========
*/
void Sys_InitInput() {
	common->Printf ("\n------- Input Initialization -------\n");
	if ( win32.in_mouse.GetBool() ) {
		IN_ActivateMouse();
		// don't grab the mouse on initialization
		Sys_GrabMouseCursor( false );
	} else {
		common->Printf ("Mouse control not active.\n");
	}
	//IN_StartupKeyboard();

	common->Printf ("------------------------------------\n");
	win32.in_mouse.ClearModified();
}

/*
=======
MapKey

Map from SDL to Doom keynums
=======
*/
int MapKey( int key ) {
	int result = 0;

	switch( key ) {
	case SDL_SCANCODE_ESCAPE:		result = K_ESCAPE;		break;
	case SDL_SCANCODE_1:			result = K_1;			break;
	case SDL_SCANCODE_2:			result = K_2;			break;
	case SDL_SCANCODE_3:			result = K_3;			break;
	case SDL_SCANCODE_4:			result = K_4;			break;
	case SDL_SCANCODE_5:			result = K_5;			break;
	case SDL_SCANCODE_6:			result = K_6;			break;
	case SDL_SCANCODE_7:			result = K_7;			break;
	case SDL_SCANCODE_8:			result = K_8;			break;
	case SDL_SCANCODE_9:			result = K_9;			break;
	case SDL_SCANCODE_0:			result = K_0;			break;
	case SDL_SCANCODE_MINUS:		result = K_MINUS;		break;
	case SDL_SCANCODE_EQUALS:		result = K_EQUALS;		break;
	case SDL_SCANCODE_BACKSPACE:	result = K_BACKSPACE;	break;
	case SDL_SCANCODE_TAB:			result = K_TAB;			break;
	case SDL_SCANCODE_Q:			result = K_Q;			break;
	case SDL_SCANCODE_W:			result = K_W;			break;
	case SDL_SCANCODE_E:			result = K_E;			break;
	case SDL_SCANCODE_R:			result = K_R;			break;
	case SDL_SCANCODE_T:			result = K_T;			break;
	case SDL_SCANCODE_Y:			result = K_Y;			break;
	case SDL_SCANCODE_U:			result = K_U;			break;
	case SDL_SCANCODE_I:			result = K_I;			break;
	case SDL_SCANCODE_O:			result = K_O;			break;
	case SDL_SCANCODE_P:			result = K_P;			break;
	case SDL_SCANCODE_LEFTBRACKET:	result = K_LBRACKET;	break;
	case SDL_SCANCODE_RIGHTBRACKET:	result = K_RBRACKET;	break;
	case SDL_SCANCODE_RETURN:		result = K_ENTER;		break;
	case SDL_SCANCODE_LCTRL:		result = K_LCTRL;		break;
	case SDL_SCANCODE_A:			result = K_A;			break;
	case SDL_SCANCODE_S:			result = K_S;			break;
	case SDL_SCANCODE_D:			result = K_D;			break;
	case SDL_SCANCODE_F:			result = K_F;			break;
	case SDL_SCANCODE_G:			result = K_G;			break;
	case SDL_SCANCODE_H:			result = K_H;			break;
	case SDL_SCANCODE_J:			result = K_J;			break;
	case SDL_SCANCODE_K:			result = K_K;			break;
	case SDL_SCANCODE_L:			result = K_L;			break;
	case SDL_SCANCODE_SEMICOLON:	result = K_SEMICOLON;	break;
	case SDL_SCANCODE_APOSTROPHE:	result = K_APOSTROPHE;	break;
	case SDL_SCANCODE_GRAVE:		result = K_GRAVE;		break;
	case SDL_SCANCODE_LSHIFT:		result = K_LSHIFT;		break;
	case SDL_SCANCODE_BACKSLASH:	result = K_BACKSLASH;	break;
	case SDL_SCANCODE_Z:			result = K_Z;			break;
	case SDL_SCANCODE_X:			result = K_X;			break;
	case SDL_SCANCODE_C:			result = K_C;			break;
	case SDL_SCANCODE_V:			result = K_V;			break;
	case SDL_SCANCODE_B:			result = K_B;			break;
	case SDL_SCANCODE_N:			result = K_N;			break;
	case SDL_SCANCODE_M:			result = K_M;			break;
	case SDL_SCANCODE_COMMA:		result = K_COMMA;		break;
	case SDL_SCANCODE_PERIOD:		result = K_PERIOD;		break;
	case SDL_SCANCODE_SLASH:		result = K_SLASH;		break;
	case SDL_SCANCODE_RSHIFT:		result = K_RSHIFT;		break;
	case SDL_SCANCODE_KP_MULTIPLY:	result = K_KP_STAR;		break;
	case SDL_SCANCODE_LALT:			result = K_LALT;		break;
	case SDL_SCANCODE_SPACE:		result = K_SPACE;		break;
	case SDL_SCANCODE_CAPSLOCK:		result = K_CAPSLOCK;	break;
	case SDL_SCANCODE_F1:			result = K_F1;			break;
	case SDL_SCANCODE_F2:			result = K_F2;			break;
	case SDL_SCANCODE_F3:			result = K_F3;			break;
	case SDL_SCANCODE_F4:			result = K_F4;			break;
	case SDL_SCANCODE_F5:			result = K_F5;			break;
	case SDL_SCANCODE_F6:			result = K_F6;			break;
	case SDL_SCANCODE_F7:			result = K_F7;			break;
	case SDL_SCANCODE_F8:			result = K_F8;			break;
	case SDL_SCANCODE_F9:			result = K_F9;			break;
	case SDL_SCANCODE_F10:			result = K_F10;			break;
	case SDL_SCANCODE_NUMLOCKCLEAR:	result = K_NUMLOCK;		break;
	case SDL_SCANCODE_SCROLLLOCK:	result = K_SCROLL;		break;
	case SDL_SCANCODE_KP_7:			result = K_KP_7;		break;
	case SDL_SCANCODE_KP_8:			result = K_KP_8;		break;
	case SDL_SCANCODE_KP_9:			result = K_KP_9;		break;
	case SDL_SCANCODE_KP_MINUS:		result = K_KP_MINUS;	break;
	case SDL_SCANCODE_KP_4:			result = K_KP_4;		break;
	case SDL_SCANCODE_KP_5:			result = K_KP_5;		break;
	case SDL_SCANCODE_KP_6:			result = K_KP_6;		break;
	case SDL_SCANCODE_KP_PLUS:		result = K_KP_PLUS;		break;
	case SDL_SCANCODE_KP_1:			result = K_KP_1;		break;
	case SDL_SCANCODE_KP_2:			result = K_KP_2;		break;
	case SDL_SCANCODE_KP_3:			result = K_KP_3;		break;
	case SDL_SCANCODE_KP_0:			result = K_KP_0;		break;
	case SDL_SCANCODE_KP_PERIOD:	result = K_KP_DOT;		break;
	case SDL_SCANCODE_F11:			result = K_F11;			break;
	case SDL_SCANCODE_F12:			result = K_F12;			break;
	case SDL_SCANCODE_F13:			result = K_F13;			break;
	case SDL_SCANCODE_F14:			result = K_F14;			break;
	case SDL_SCANCODE_F15:			result = K_F15;			break;
	case SDL_SCANCODE_KP_EQUALS:	result = K_KP_EQUALS;	break;
	case SDL_SCANCODE_STOP:			result = K_STOP;		break;
	case SDL_SCANCODE_KP_ENTER:		result = K_KP_ENTER;	break;
	case SDL_SCANCODE_RCTRL:		result = K_RCTRL;		break;
	case SDL_SCANCODE_KP_COMMA:		result = K_KP_COMMA;	break;
	case SDL_SCANCODE_KP_DIVIDE:	result = K_KP_SLASH;	break;
	case SDL_SCANCODE_PRINTSCREEN:	result = K_PRINTSCREEN;	break;
	case SDL_SCANCODE_RALT:			result = K_RALT;		break;
	case SDL_SCANCODE_PAUSE:		result = K_PAUSE;		break;
	case SDL_SCANCODE_HOME:			result = K_HOME;		break;
	case SDL_SCANCODE_UP:			result = K_UPARROW;		break;
	case SDL_SCANCODE_PAGEUP:		result = K_PGUP;		break;
	case SDL_SCANCODE_LEFT:			result = K_LEFTARROW;	break;
	case SDL_SCANCODE_RIGHT:		result = K_RIGHTARROW;	break;
	case SDL_SCANCODE_END:			result = K_END;			break;
	case SDL_SCANCODE_DOWN:			result = K_DOWNARROW;	break;
	case SDL_SCANCODE_PAGEDOWN:		result = K_PGDN;		break;
	case SDL_SCANCODE_INSERT:		result = K_INS;			break;
	case SDL_SCANCODE_DELETE:		result = K_DEL;			break;
	case SDL_SCANCODE_LGUI:			result = K_LWIN;		break;
	case SDL_SCANCODE_RGUI:			result = K_RWIN;		break;
	case SDL_SCANCODE_APPLICATION:	result = K_APPS;		break;
	case SDL_SCANCODE_POWER:		result = K_POWER;		break;
	case SDL_SCANCODE_SLEEP:		result = K_SLEEP;		break;
	default:						result = K_NONE;		break;
	}

	return result;
}

/*
====================
IN_PollEvents
====================
*/
void IN_PollEvents() {
	SDL_Event event;
	int key;

	poll_mouse_event_count = 0;

	while( SDL_PollEvent( &event ) ) {
		switch( event.type ) {
		case SDL_WINDOWEVENT:
			switch( event.window.event ) {
			case SDL_WINDOWEVENT_MOVED:
				if ( !renderSystem->IsFullScreen() ) {
					r_windowX.SetInteger( event.window.data1 );
					r_windowY.SetInteger( event.window.data2 );
				}
				break;

			case SDL_WINDOWEVENT_LEAVE:
				Sys_QueEvent( SE_MOUSE_LEAVE, 0, 0, 0, NULL, 0 );
				break;

			case SDL_WINDOWEVENT_FOCUS_GAINED:
			case SDL_WINDOWEVENT_FOCUS_LOST:
				// if we got here because of an alt-tab or maximize,
				// we should activate immediately.  If we are here because
				// the mouse was clicked on a title bar or drag control,
				// don't activate until the mouse button is released
				{
					win32.activeApp = ( event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED );
					if ( win32.activeApp ) {
						idKeyInput::ClearStates();
						Sys_GrabMouseCursor( true );
						if ( common->IsInitialized() ) {
							SDL_ShowCursor( SDL_DISABLE );
						}
					}

					if ( event.window.event == SDL_WINDOWEVENT_FOCUS_LOST ) {
						win32.movingWindow = false;
						if ( common->IsInitialized() ) {
							SDL_ShowCursor( SDL_ENABLE );
						}
					}

					// start playing the game sound world
					soundSystem->SetMute( !win32.activeApp );

					// we do not actually grab or release the mouse here,
					// that will be done next time through the main loop
				}
				break;

			case SDL_WINDOWEVENT_RESIZED:
				if ( !R_IsInitialized() || renderSystem->GetWidth() <= 0 || renderSystem->GetHeight() <= 0 ) {
					return;
				}

				// restrict to a standard aspect ratio
				int width = event.window.data1;
				int height = event.window.data2;

				// Clamp to a minimum size
				if ( width < SCREEN_WIDTH / 4 ) {
					width = SCREEN_WIDTH / 4;
				}
				if ( height < SCREEN_HEIGHT / 4 ) {
					height = SCREEN_HEIGHT / 4;
				}

				const int minWidth = height * 4 / 3;
				const int maxWidth = height * 16 / 9;

				width = idMath::ClampInt( minWidth, maxWidth, width );

				glConfig.nativeScreenWidth = width;
				glConfig.nativeScreenHeight = height;

				// save the window size in cvars if we aren't fullscreen
				if ( !renderSystem->IsFullScreen() ) {
					SDL_SetWindowSize( win32.window, width, height );
					r_windowWidth.SetInteger( width );
					r_windowHeight.SetInteger( height );
				}
				break;
			}
			break;

		case SDL_KEYDOWN:
			if ( event.key.keysym.mod & KMOD_ALT && event.key.keysym.scancode == SDL_SCANCODE_RETURN ) {	// alt-enter toggles full-screen
				cvarSystem->SetCVarBool( "r_fullscreen", !renderSystem->IsFullScreen() );
				cmdSystem->BufferCommandText( CMD_EXEC_APPEND, "vid_restart\n" );
				break;
			}

			key = MapKey( event.key.keysym.scancode );
			// D
			if ( key == K_NUMLOCK ) {
				key = K_PAUSE;
			} else if ( key == K_PAUSE ) {
				key = K_NUMLOCK;
			}
			Sys_QueEvent( SE_KEY, key, true, 0, NULL, 0 );
			PostKeyboardEvent( key, true );

			if ( key == K_BACKSPACE ) {
				Sys_QueEvent( SE_CHAR, event.key.keysym.sym, 0, 0, NULL, 0 );
			} else if ( event.key.keysym.mod & KMOD_CTRL && event.key.keysym.sym >= 'a' && event.key.keysym.sym <= 'z' ) {
				Sys_QueEvent( SE_CHAR, ( event.key.keysym.sym - 'a' + 1 ), 0, 0, NULL, 0 );
			}
			break;

		case SDL_KEYUP:
			key = MapKey( event.key.keysym.scancode );
			Sys_QueEvent( SE_KEY, key, false, 0, NULL, 0 );
			PostKeyboardEvent( key, false );
			break;

		case SDL_TEXTINPUT: {
			char *c = event.text.text;

			// Quick and dirty UTF-8 to UTF-32 conversion
			while( *c ) {
				int utf32 = 0;

				if( ( *c & 0x80 ) == 0 ) {
					utf32 = *c++;
				} else if( ( *c & 0xE0 ) == 0xC0 ) { // 110x xxxx
					utf32 |= ( *c++ & 0x1F ) << 6;
					utf32 |= ( *c++ & 0x3F );
				} else if( ( *c & 0xF0 ) == 0xE0 ) { // 1110 xxxx
					utf32 |= ( *c++ & 0x0F ) << 12;
					utf32 |= ( *c++ & 0x3F ) << 6;
					utf32 |= ( *c++ & 0x3F );
				} else if( ( *c & 0xF8 ) == 0xF0 ) { // 1111 0xxx
					utf32 |= ( *c++ & 0x07 ) << 18;
					utf32 |= ( *c++ & 0x3F ) << 12;
					utf32 |= ( *c++ & 0x3F ) << 6;
					utf32 |= ( *c++ & 0x3F );
				} else {
					common->DPrintf( "Unrecognised UTF-8 lead byte: 0x%x\n", (unsigned int)*c );
					c++;
				}

				if( utf32 != 0 ) {
					Sys_QueEvent( SE_CHAR, utf32, 0, 0, NULL, 0 );
				}
			}
			
			break;
		}

		case SDL_QUIT:
			soundSystem->SetMute( true );
			cmdSystem->BufferCommandText( CMD_EXEC_APPEND, "quit\n" );
			break;

		case SDL_MOUSEMOTION: {
			if ( !common->IsInitialized() ) {
				break;
			}

			const bool isShellActive = ( game && ( game->Shell_IsActive() || game->IsPDAOpen() ) );
			const bool isConsoleActive = console->Active();

			if ( win32.activeApp ) {
				if ( isShellActive ) {
					// If the shell is active, it will display its own cursor.
					SDL_ShowCursor( SDL_DISABLE );
				} else if ( isConsoleActive ) {
					// The console is active but the shell is not.
					// Show the Windows cursor.
					SDL_ShowCursor( SDL_ENABLE );
				} else {
					// The shell not active and neither is the console.
					// This is normal gameplay, hide the cursor.
					SDL_ShowCursor( SDL_DISABLE );
				}
			} else {
				if ( !isShellActive ) {
					// Always show the cursor when the window is in the background
					SDL_ShowCursor( SDL_ENABLE );
				} else {
					SDL_ShowCursor( SDL_DISABLE );
				}
			}

			// Generate an event
			PostMouseEvent( M_DELTAX, event.motion.xrel );
			PostMouseEvent( M_DELTAY, event.motion.yrel );
			Sys_QueEvent( SE_MOUSE_ABSOLUTE, event.motion.x, event.motion.y, 0, NULL, 0 );
			break;
		}

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP: {
			int b;
			switch( event.button.button ) {
			case SDL_BUTTON_LEFT:
				b = K_MOUSE1;
				break;
			case SDL_BUTTON_MIDDLE:
				b = K_MOUSE3;
				break;
			case SDL_BUTTON_RIGHT:
				b = K_MOUSE2;
				break;
			case SDL_BUTTON_X1:
				b = K_MOUSE4;
				break;
			case SDL_BUTTON_X2:
				b = K_MOUSE5;
				break;
			default:
				b = NULL;
				break;
			}
			bool buttonDown = ( event.type == SDL_MOUSEBUTTONDOWN );
			PostMouseEvent( b - K_MOUSE1, buttonDown );
			Sys_QueEvent( SE_KEY, b, buttonDown, 0, NULL, 0 );
			break;
		}

		case SDL_MOUSEWHEEL: {
			int delta = event.wheel.y;
			int key = delta < 0 ? K_MWHEELDOWN : K_MWHEELUP;
			PostMouseEvent( M_DELTAZ, delta );
			delta = abs( delta );
			while( delta-- > 0 ) {
				Sys_QueEvent( SE_KEY, key, true, 0, NULL, 0 );
				Sys_QueEvent( SE_KEY, key, false, 0, NULL, 0 );
			}
			break;
		}

		case SDL_CONTROLLERDEVICEADDED:
		case SDL_CONTROLLERDEVICEREMOVED:
			win32.g_Joystick.Init();
			break;

		}
	}
}

/*
==================
IN_Frame

Called every frame, even if not generating commands
==================
*/
void IN_Frame() {
	bool	shouldGrab = true;

	if ( !win32.in_mouse.GetBool() ) {
		shouldGrab = false;
	}
	// if fullscreen, we always want the mouse
	if ( !win32.cdsFullscreen ) {
		if ( win32.mouseReleased ) {
			shouldGrab = false;
		}
		if ( win32.movingWindow ) {
			shouldGrab = false;
		}
		if ( !win32.activeApp ) {
			shouldGrab = false;
		}
	}

	if ( shouldGrab != win32.mouseGrabbed ) {
		if ( usercmdGen != NULL ) {
			usercmdGen->Clear();
		}

		if ( win32.mouseGrabbed ) {
			IN_DeactivateMouse();
		} else {
			IN_ActivateMouse();
		}
	}

	IN_PollEvents();
}


void	Sys_GrabMouseCursor( bool grabIt ) {
	win32.mouseReleased = !grabIt;
	if ( !grabIt ) {
		// release it right now
		IN_Frame();
	}
}

//=====================================================================================


/*
====================
Sys_PollKeyboardInputEvents
====================
*/
int Sys_PollKeyboardInputEvents() {
	return poll_keyboard_event_count;
}

/*
====================
Sys_PollKeyboardInputEvents
====================
*/
int Sys_ReturnKeyboardInputEvent( const int n, int &ch, bool &state ) {
	if( n >= poll_keyboard_event_count ) {
		return 0;
	}
	ch = poll_events_keyboard[n].event;
	state = ( poll_events_keyboard[n].value != 0 );
	return 1;
}


void Sys_EndKeyboardInputEvents() {
	poll_keyboard_event_count = 0;	
}

//=====================================================================================


int Sys_PollMouseInputEvents( int mouseEvents[MAX_MOUSE_EVENTS][2] ) {
	if ( !win32.mouseGrabbed ) {
		return 0;
	}

	if ( poll_mouse_event_count > MAX_MOUSE_EVENTS ) {
		poll_mouse_event_count = MAX_MOUSE_EVENTS;
	}

	int i;
	for( i = 0; i < poll_mouse_event_count; i++ ) {
		mouseEvents[i][0] = poll_events_mouse[i].event;
		mouseEvents[i][1] = poll_events_mouse[i].value;
	}

	return i;
}

//=====================================================================================
//	Joystick Input Handling
//=====================================================================================

void Sys_SetRumble( int device, int low, int hi ) {
	return win32.g_Joystick.SetRumble( device, low, hi );
}

int Sys_PollJoystickInputEvents( int deviceNum ) {
	return win32.g_Joystick.PollInputEvents( deviceNum );
}


int Sys_ReturnJoystickInputEvent( const int n, int &action, int &value ) {
	return win32.g_Joystick.ReturnInputEvent( n, action, value );
}


void Sys_EndJoystickInputEvents() {
}


/*
========================
idJoystickSDL::idJoystickSDL
========================
*/
idJoystickSDL::idJoystickSDL() {
	numEvents = 0;
	memset( &events, 0, sizeof( events ) );
	memset( &controllers, 0, sizeof( controllers ) );
	memset( buttonStates, 0, sizeof( buttonStates ) );
	memset( joyAxis, 0, sizeof( joyAxis ) );
}

/*
========================
idJoystickSDL::Init
========================
*/
bool idJoystickSDL::Init() {
	idJoystick::Init();

	if ( !SDL_WasInit( SDL_INIT_JOYSTICK ) ) {
		common->DPrintf("Calling SDL_Init(SDL_INIT_JOYSTICK)...\n");
		if ( SDL_Init( SDL_INIT_JOYSTICK ) != 0 ) {
			common->DPrintf( "SDL_Init(SDL_INIT_JOYSTICK) failed: %s\n", SDL_GetError() );
			return false;
		}
		common->DPrintf( "SDL_Init(SDL_INIT_JOYSTICK) passed.\n" );
	}

	if ( !SDL_WasInit( SDL_INIT_GAMECONTROLLER ) ) {
		common->DPrintf( "Calling SDL_Init(SDL_INIT_GAMECONTROLLER)...\n" );
		if ( SDL_Init( SDL_INIT_GAMECONTROLLER ) != 0 ) {
			common->DPrintf( "SDL_Init(SDL_INIT_GAMECONTROLLER) failed: %s\n", SDL_GetError() );
			return false;
		}
		common->DPrintf( "SDL_Init(SDL_INIT_GAMECONTROLLER) passed.\n" );
	}

	if ( !SDL_WasInit( SDL_INIT_HAPTIC ) ) {
		common->DPrintf( "Calling SDL_Init(SDL_INIT_HAPTIC)...\n" );
		if ( SDL_Init( SDL_INIT_HAPTIC ) != 0 ) {
			common->DPrintf( "SDL_Init(SDL_INIT_HAPTIC) failed: %s\n", SDL_GetError() );
			return false;
		}
		common->DPrintf( "SDL_Init(SDL_INIT_HAPTIC) passed.\n" );
	}

	if ( SDL_NumJoysticks() == 0 ) {
		return false;
	}

	for ( int i = 0; i < SDL_NumJoysticks(); i++ ) {
		if ( SDL_IsGameController( i ) ) {
			controller = SDL_GameControllerOpen( i );
			if ( controller == NULL ) {
				common->DPrintf( "Could not open gamecontroller %i: %s\n", i, SDL_GetError() );
			}
			haptic = SDL_HapticOpen( i );
			if ( haptic == NULL ) {
				common->DPrintf( "Could not open haptic for gamecontroller %i: %s\n", i, SDL_GetError() );
			}
		}
	}

	return true;
}

/*
========================
idJoystickSDL::SetRumble
========================
*/
void idJoystickSDL::SetRumble( int inputDeviceNum, int rumbleLow, int rumbleHigh ) {
	if ( inputDeviceNum < 0 || inputDeviceNum >= MAX_JOYSTICKS ) {
		return;
	}
	if ( !controllers[inputDeviceNum].valid ) {
//		return;
	}
	SDL_HapticEffect effect;
	static int effectId = NULL;
	SDL_HapticDestroyEffect( haptic, effectId );
	if ( rumbleLow == 0 && rumbleHigh == 0) {
		return;
	}
	// Create the effect
	memset( &effect, 0, sizeof(SDL_HapticEffect) ); // 0 is safe default
	effect.type = SDL_HAPTIC_LEFTRIGHT;
	effect.leftright.length = SDL_HAPTIC_INFINITY;
	effect.leftright.large_magnitude = idMath::ClampInt( 0, 32767, rumbleLow / 2 );
	effect.leftright.small_magnitude = idMath::ClampInt( 0, 32767, rumbleHigh / 2 );
	// Upload the effect
	effectId = SDL_HapticNewEffect( haptic, &effect );
	// Run the effect
	SDL_HapticRunEffect( haptic, effectId, 1 );
}

/*
========================
idJoystickSDL::PostInputEvent
========================
*/
void idJoystickSDL::PostInputEvent( int inputDeviceNum, int event, int value, int range ) {
	// These events are used for GUI button presses
	if ( ( event >= J_ACTION1 ) && ( event <= J_ACTION_MAX ) ) {
		PushButton( inputDeviceNum, K_JOY1 + ( event - J_ACTION1 ), value != 0 );
	} else if ( event == J_AXIS_LEFT_X ) {
		PushButton( inputDeviceNum, K_JOY_STICK1_LEFT, ( value < -range ) );
		PushButton( inputDeviceNum, K_JOY_STICK1_RIGHT, ( value > range ) );
	} else if ( event == J_AXIS_LEFT_Y ) {
		PushButton( inputDeviceNum, K_JOY_STICK1_UP, ( value < -range ) );
		PushButton( inputDeviceNum, K_JOY_STICK1_DOWN, ( value > range ) );
	} else if ( event == J_AXIS_RIGHT_X ) {
		PushButton( inputDeviceNum, K_JOY_STICK2_LEFT, ( value < -range ) );
		PushButton( inputDeviceNum, K_JOY_STICK2_RIGHT, ( value > range ) );
	} else if ( event == J_AXIS_RIGHT_Y ) {
		PushButton( inputDeviceNum, K_JOY_STICK2_UP, ( value < -range ) );
		PushButton( inputDeviceNum, K_JOY_STICK2_DOWN, ( value > range ) );
	} else if ( ( event >= J_DPAD_UP ) && ( event <= J_DPAD_RIGHT ) ) {
		PushButton( inputDeviceNum, K_JOY_DPAD_UP + ( event - J_DPAD_UP ), value != 0 );
	} else if ( event == J_AXIS_LEFT_TRIG ) {
		PushButton( inputDeviceNum, K_JOY_TRIGGER1, ( value > range ) );
	} else if ( event == J_AXIS_RIGHT_TRIG ) {
		PushButton( inputDeviceNum, K_JOY_TRIGGER2, ( value > range ) );
	}
	if ( event >= J_AXIS_MIN && event <= J_AXIS_MAX ) {
		int axis = event - J_AXIS_MIN;
		int percent = ( value * 16 ) / range;
		if ( joyAxis[inputDeviceNum][axis] != percent ) {
			joyAxis[inputDeviceNum][axis] = percent;
			Sys_QueEvent( SE_JOYSTICK, axis, percent, 0, NULL, inputDeviceNum );
		}
	}

	// These events are used for actual game input
	events[numEvents].event = event;
	events[numEvents].value = value;
	numEvents++;
}

/*
========================
idJoystickSDL::PollInputEvents
========================
*/
int idJoystickSDL::PollInputEvents( int inputDeviceNum ) {
	numEvents = 0;

	if ( !win32.activeApp ) {
		return numEvents;
	}

//	assert( inputDeviceNum < 4 );

//	if ( inputDeviceNum > in_joystick.GetInteger() ) {
//		return numEvents;
//	}
	
	SDL_GameControllerUpdate();

	controllerState_t *cs = &controllers[ inputDeviceNum ];

	if ( session->IsSystemUIShowing() ) {
		// memset xis so the current input does not get latched if the UI is showing
		//memset( &xis, 0, sizeof( XINPUT_STATE ) );
	}

	int buttonMap[SDL_CONTROLLER_BUTTON_MAX] = {
		J_ACTION1,		// A
		J_ACTION2,		// B
		J_ACTION3,		// X
		J_ACTION4,		// Y
		J_ACTION10,		// Back
		0,				// Unused
		J_ACTION9,		// Start
		J_ACTION7,		// Left Stick Down
		J_ACTION8,		// Right Stick Down
		J_ACTION5,		// Black (Left Shoulder)
		J_ACTION6,		// White (Right Shoulder)
		J_DPAD_UP,		// Up
		J_DPAD_DOWN,	// Down
		J_DPAD_LEFT,	// Left
		J_DPAD_RIGHT,	// Right
	};

	// Check the digital buttons
	for ( int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++ ) {
		bool pressed = ( SDL_GameControllerGetButton( controller, (SDL_GameControllerButton)( SDL_CONTROLLER_BUTTON_A + i ) ) > 0 );
		if ( pressed != cs->buttons[i] ) {
			PostInputEvent( inputDeviceNum, buttonMap[i], pressed );
			cs->buttons[i] = pressed;
		}
	}

	int axisMap[SDL_CONTROLLER_AXIS_MAX] = {
		J_AXIS_LEFT_X,
		J_AXIS_LEFT_Y,
		J_AXIS_RIGHT_X,
		J_AXIS_RIGHT_Y,
		J_AXIS_LEFT_TRIG,
		J_AXIS_RIGHT_TRIG,
	};

	// Check the axes
	for ( int i = 0; i < SDL_CONTROLLER_AXIS_MAX; i++ ) {
		int axis = SDL_GameControllerGetAxis( controller, (SDL_GameControllerAxis)i );
		if ( axis != cs->axes[i] ) {
			PostInputEvent( inputDeviceNum, axisMap[i], axis );
			cs->axes[i] = axis;
		}
	}

	return numEvents;
}


/*
========================
idJoystickSDL::ReturnInputEvent
========================
*/
int idJoystickSDL::ReturnInputEvent( const int n, int & action, int &value ) {
	if ( ( n < 0 ) || ( n >= MAX_JOY_EVENT ) ) {
		return 0;
	}

	action = events[ n ].event;
	value = events[ n ].value;

	return 1;
}

/*
========================
idJoystickSDL::PushButton
========================
*/
void idJoystickSDL::PushButton( int inputDeviceNum, int key, bool value ) {
	// So we don't keep sending the same SE_KEY message over and over again
	if ( buttonStates[inputDeviceNum][key] != value ) {
		buttonStates[inputDeviceNum][key] = value;
		Sys_QueEvent( SE_KEY, key, value, 0, NULL, inputDeviceNum );
	}
}
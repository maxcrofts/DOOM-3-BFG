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

#include "Precompiled.h"
#include "globaldata.h"

//
// DESCRIPTION:
//	System interface for sound.
//
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <sys/types.h>
#include <fcntl.h>
// Timer stuff. Experimental.
#include <time.h>
#include <signal.h>
#include "z_zone.h"
#include "i_system.h"
#include "i_sound.h"
#include "m_argv.h"
#include "m_misc.h"
#include "w_wad.h"
#include "d_main.h"
#include "doomdef.h"
#include "../timidity/timidity.h"
#include "../timidity/controls.h"

#include "sound/snd_local.h"

#include <FAudio.h>
#include <F3DAudio.h>

#pragma warning ( disable : 4244 )

#define	MIDI_CHANNELS		2
#define MIDI_RATE			22050
#define MIDI_FORMAT			AUDIO_U8
#define MIDI_FORMAT_BYTES	1

FAudioSourceVoice*	pMusicSourceVoice;
MidiSong*				doomMusic;
byte*					musicBuffer;
int						totalBufferSize;

bool	waitingForMusic;
bool	musicReady;


typedef struct tagActiveSound_t {
	FAudioSourceVoice*     m_pSourceVoice;         // Source voice
	F3DAUDIO_DSP_SETTINGS   m_DSPSettings;
	F3DAUDIO_EMITTER        m_Emitter;
	F3DAUDIO_CONE           m_Cone;
	int id;
	int valid;
	int start;
	int player;
	bool localSound;
	mobj_t *originator;
} activeSound_t;


// cheap little struct to hold a sound
typedef struct {
	int vol;
	int player;
	int pitch;
	int priority;
	mobj_t *originator;
	mobj_t *listener;
} soundEvent_t;

// array of all the possible sounds
// in split screen we only process the loudest sound of each type per frame
soundEvent_t soundEvents[128];
extern int PLAYERCOUNT;

// Real volumes
const float		GLOBAL_VOLUME_MULTIPLIER = 0.5f;

float			x_SoundVolume = GLOBAL_VOLUME_MULTIPLIER;
float			x_MusicVolume = GLOBAL_VOLUME_MULTIPLIER;

// The actual lengths of all sound effects.
static int 		lengths[NUMSFX];
activeSound_t	activeSounds[NUM_SOUNDBUFFERS] = {0};

int				S_initialized = 0;
bool			Music_initialized = false;

// FAUDIO
float			g_EmitterAzimuths [] = { 0.f };
static int		numOutputChannels = 0;
static bool		soundHardwareInitialized = false;


F3DAUDIO_HANDLE					F3DAudioInstance;

F3DAUDIO_LISTENER				doom_Listener;

//float							localSoundVolumeEntries[] = { 0.f, 0.f, 0.9f, 0.5f, 0.f, 0.f };
float							localSoundVolumeEntries[] = { 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f };


void							I_InitSoundChannel( int channel, int numOutputChannels_ );

/*
======================
getsfx
======================
*/
// This function loads the sound data from the WAD lump,
//  for single sound.
//
void* getsfx ( const char* sfxname, int* len )
{
	unsigned char*      sfx;
	unsigned char*	    sfxmem;
	int                 size;
	char                name[20];
	int                 sfxlump;
	float				scale = 1.0f;

	// Get the sound data from the WAD, allocate lump
	//  in zone memory.
	sprintf(name, "ds%s", sfxname);

	// Scale down the plasma gun, it clips
	if ( strcmp( sfxname, "plasma" ) == 0 ) {
		scale = 0.75f;
	}
	if ( strcmp( sfxname, "itemup" ) == 0 ) {
		scale = 1.333f;
	}

	// If sound requested is not found in current WAD, use pistol as default
	if ( W_CheckNumForName(name) == -1 )
		sfxlump = W_GetNumForName("dspistol");
	else
		sfxlump = W_GetNumForName(name);

	// Sound lump headers are 8 bytes.
	const int SOUND_LUMP_HEADER_SIZE_IN_BYTES = 8;

	size = W_LumpLength( sfxlump ) - SOUND_LUMP_HEADER_SIZE_IN_BYTES;

	sfx = (unsigned char*)W_CacheLumpNum( sfxlump, PU_CACHE_SHARED );
	const unsigned char * sfxSampleStart = sfx + SOUND_LUMP_HEADER_SIZE_IN_BYTES;

	// Allocate from zone memory.
	//sfxmem = (float*)DoomLib::Z_Malloc( size*(sizeof(float)), PU_SOUND_SHARED, 0 );
	sfxmem = (unsigned char*)malloc( size * sizeof(unsigned char) );

	// Now copy, and convert to Xbox360 native float samples, do initial volume ramp, and scale
	for ( int i=0; i<size; i++ ) {
		sfxmem[i] = sfxSampleStart[i];// * scale;
	}

	// Remove the cached lump.
	Z_Free( sfx );

	// Set length.
	*len = size;

	// Return allocated padded data.
	return (void *) (sfxmem);
}

/*
======================
I_SetChannels
======================
*/
void I_SetChannels() {
	// Original Doom set up lookup tables here
}	

/*
======================
I_SetSfxVolume
======================
*/
void I_SetSfxVolume(int volume) {
	x_SoundVolume = ((float)volume / 15.f) * GLOBAL_VOLUME_MULTIPLIER;
}

/*
======================
I_GetSfxLumpNum
======================
*/
//
// Retrieve the raw data lump index
//  for a given SFX name.
//
int I_GetSfxLumpNum(sfxinfo_t* sfx)
{
	char namebuf[9];
	sprintf(namebuf, "ds%s", sfx->name);
	return W_GetNumForName(namebuf);
}

/*
======================
I_StartSound2
======================
*/
// Starting a sound means adding it
//  to the current list of active sounds
//  in the internal channels.
// As the SFX info struct contains
//  e.g. a pointer to the raw data,
//  it is ignored.
// As our sound handling does not handle
//  priority, it is ignored.
// Pitching (that is, increased speed of playback) is set
//
int I_StartSound2 ( int id, int player, mobj_t *origin, mobj_t *listener_origin, int pitch, int priority ) {
	if ( !soundHardwareInitialized ) {
		return id;
	}
	
	int i;
	FAudioVoiceState state;
	activeSound_t* sound = 0;
	int oldest = 0, oldestnum = -1;

	// these id's should not overlap
	if ( id == sfx_sawup || id == sfx_sawidl || id == sfx_sawful || id == sfx_sawhit || id == sfx_stnmov ) {
		// Loop all channels, check.
		for (i=0 ; i < NUM_SOUNDBUFFERS ; i++)
		{
			sound = &activeSounds[i];

			if (sound->valid && ( sound->id == id && sound->player == player ) ) {
				I_StopSound( sound->id, player );
				break;
			}
		}
	}

	// find a valid channel, or one that has finished playing
	for (i = 0; i < NUM_SOUNDBUFFERS; ++i) {
		sound = &activeSounds[i];
		
		if (!sound->valid)
			break;

		if (!oldest || oldest > sound->start) {
			oldestnum = i;
			oldest = sound->start;
		}

		FAudioSourceVoice_GetState( sound->m_pSourceVoice, &state, 0 );
		if ( state.BuffersQueued == 0 ) {
			break;
		}
	}

	// none found, so use the oldest one
	if (i == NUM_SOUNDBUFFERS)
	{
		i = oldestnum;
		sound = &activeSounds[i];
	}

	// stop the sound with a FlushPackets
	FAudioSourceVoice_Stop( sound->m_pSourceVoice, 0, FAUDIO_COMMIT_NOW );
	FAudioSourceVoice_FlushSourceBuffers( sound->m_pSourceVoice );

	// Set up packet
	FAudioBuffer Packet = { 0 };
	Packet.Flags = FAUDIO_END_OF_STREAM;
	Packet.AudioBytes = lengths[id];
	Packet.pAudioData = (unsigned char*)S_sfx[id].data;
	Packet.PlayBegin = 0;
	Packet.PlayLength = 0;
	Packet.LoopBegin = FAUDIO_NO_LOOP_REGION;
	Packet.LoopLength = 0;
	Packet.LoopCount = 0;
	Packet.pContext = NULL;


	// Set voice volumes
	FAudioVoice_SetVolume( sound->m_pSourceVoice, x_SoundVolume, FAUDIO_COMMIT_NOW );

	// Set voice pitch
	FAudioSourceVoice_SetFrequencyRatio( sound->m_pSourceVoice, 1 + ((float)pitch-128.f)/95.f, FAUDIO_COMMIT_NOW );

	// Set initial spatialization
	if ( origin && origin != listener_origin ) {
		// Update Emitter Position
		sound->m_Emitter.Position.x = (float)(origin->x >> FRACBITS);
		sound->m_Emitter.Position.y = 0.f;
		sound->m_Emitter.Position.z = (float)(origin->y >> FRACBITS);

		// Calculate 3D positioned speaker volumes
		unsigned int calculateFlags = F3DAUDIO_CALCULATE_MATRIX;
		F3DAudioCalculate( F3DAudioInstance, &doom_Listener, &sound->m_Emitter, calculateFlags, &sound->m_DSPSettings );

		// Pan the voice according to F3DAudio calculation
		FAudioVoice_SetOutputMatrix( sound->m_pSourceVoice, NULL, 1, numOutputChannels, sound->m_DSPSettings.pMatrixCoefficients, FAUDIO_COMMIT_NOW );

		sound->localSound = false;
	} else {
		// Local(or Global) sound, fixed speaker volumes
		FAudioVoice_SetOutputMatrix( sound->m_pSourceVoice, NULL, 1, numOutputChannels, localSoundVolumeEntries, FAUDIO_COMMIT_NOW );

		sound->localSound = true;
	}

	// Submit packet
	FAudioSourceVoice_SubmitSourceBuffer( sound->m_pSourceVoice, &Packet, NULL );

	// Play the source voice
	FAudioSourceVoice_Start( sound->m_pSourceVoice, 0, FAUDIO_COMMIT_NOW );

	// set id, and start time
	sound->id = id;
	sound->start = ::g->gametic;
	sound->valid = 1;
	sound->player = player;
	sound->originator = origin;

	return id;
}

/*
======================
I_ProcessSoundEvents
======================
*/
void I_ProcessSoundEvents( void ) {
	for( int i = 0; i < 128; i++ ) {
		if( soundEvents[i].pitch ) {
			I_StartSound2( i, soundEvents[i].player, soundEvents[i].originator, soundEvents[i].listener, soundEvents[i].pitch, soundEvents[i].priority );
		}
	}
	memset( soundEvents, 0, sizeof( soundEvents ) );
}

/*
======================
I_StartSound
======================
*/
int I_StartSound ( int id, mobj_t *origin, mobj_t *listener_origin, int vol, int pitch, int priority ) {
	// only allow player 0s sounds in intermission and finale screens
	if( ::g->gamestate != GS_LEVEL && DoomLib::GetPlayer() != 0 ) {
		return 0;
	}

	// if we're only one player or we're trying to play the chainsaw sound, do it normal
	// otherwise only allow one sound of each type per frame
	if( PLAYERCOUNT == 1 || id == sfx_sawup || id == sfx_sawidl || id == sfx_sawful || id == sfx_sawhit ) {
		return I_StartSound2( id, ::g->consoleplayer, origin, listener_origin, pitch, priority );
	}
	else {
		if( soundEvents[ id ].vol < vol ) {
			soundEvents[ id ].player = DoomLib::GetPlayer();
			soundEvents[ id ].pitch = pitch;
			soundEvents[ id ].priority = priority;
			soundEvents[ id ].vol = vol;
			soundEvents[ id ].originator = origin;
			soundEvents[ id ].listener = listener_origin;
		}
		return id;
	}
}

/*
======================
I_StopSound
======================
*/
void I_StopSound (int handle, int player)
{
	// You need the handle returned by StartSound.
	// Would be looping all channels,
	//  tracking down the handle,
	//  and setting the channel to zero.

	int i;
	activeSound_t* sound = 0;

	for (i = 0; i < NUM_SOUNDBUFFERS; ++i)
	{
		sound = &activeSounds[i];
		if (!sound->valid || sound->id != handle || (player >= 0 && sound->player != player) )
			continue;
		break;
	}

	if (i == NUM_SOUNDBUFFERS)
		return;

	// stop the sound
	if ( sound->m_pSourceVoice != NULL ) {
		FAudioSourceVoice_Stop( sound->m_pSourceVoice, 0, FAUDIO_COMMIT_NOW );
	}

	sound->valid = 0;
	sound->player = -1;
}

/*
======================
I_SoundIsPlaying
======================
*/
int I_SoundIsPlaying(int handle) {
	if ( !soundHardwareInitialized ) {
		return 0;
	}

	int i;
	FAudioVoiceState	state;
	activeSound_t* sound;

	for (i = 0; i < NUM_SOUNDBUFFERS; ++i)
	{
		sound = &activeSounds[i];
		if (!sound->valid || sound->id != handle)
			continue;

		FAudioSourceVoice_GetState( sound->m_pSourceVoice, &state, 0 );
		if ( state.BuffersQueued > 0 ) {
			return 1;
		}
	}

	return 0;
}

/*
======================
I_UpdateSound
======================
*/
// Update Listener Position and go through all the
// channels and update speaker volumes for 3D sound.
void I_UpdateSound( void ) {
	if ( !soundHardwareInitialized ) {
		return;
	}

	int i;
	FAudioVoiceState	state;
	activeSound_t* sound;

	for ( i=0; i < NUM_SOUNDBUFFERS; i++ ) {
		sound = &activeSounds[i];

		if ( !sound->valid || sound->localSound ) {
			continue;
		}

		FAudioSourceVoice_GetState( sound->m_pSourceVoice, &state, 0 );

		if ( state.BuffersQueued > 0 ) {
			mobj_t *playerObj = ::g->players[ sound->player ].mo;

			// Update Listener Orientation and Position
			angle_t	pAngle = playerObj->angle;
			fixed_t fx, fz;

			pAngle >>= ANGLETOFINESHIFT;

			fx = finecosine[pAngle];
			fz = finesine[pAngle];

			doom_Listener.OrientFront.x = (float)(fx) / 65535.f;
			doom_Listener.OrientFront.y = 0.f;
			doom_Listener.OrientFront.z = (float)(fz) / 65535.f;

			// Normalize Listener Orientation
			float length = sqrtf( doom_Listener.OrientFront.x * doom_Listener.OrientFront.x + doom_Listener.OrientFront.z * doom_Listener.OrientFront.z );
			doom_Listener.OrientFront.x /= length;
			doom_Listener.OrientFront.z /= length;

			doom_Listener.Position.x = (float)(playerObj->x >> FRACBITS);
			doom_Listener.Position.y = 0.f;
			doom_Listener.Position.z = (float)(playerObj->y >> FRACBITS);

			// Update Emitter Position
			sound->m_Emitter.Position.x = (float)(sound->originator->x >> FRACBITS);
			sound->m_Emitter.Position.y = 0.f;
			sound->m_Emitter.Position.z = (float)(sound->originator->y >> FRACBITS);

			// Calculate 3D positioned speaker volumes
			unsigned int calculateFlags = F3DAUDIO_CALCULATE_MATRIX;
			F3DAudioCalculate( F3DAudioInstance, &doom_Listener, &sound->m_Emitter, calculateFlags, &sound->m_DSPSettings );

			// Pan the voice according to F3DAudio calculation
			FAudioVoice_SetOutputMatrix( sound->m_pSourceVoice, NULL, 1, numOutputChannels, sound->m_DSPSettings.pMatrixCoefficients, FAUDIO_COMMIT_NOW );
		}
	}
}

/*
======================
I_UpdateSoundParams
======================
*/
void I_UpdateSoundParams( int handle, int vol, int sep, int pitch) {
}

/*
======================
I_ShutdownSound
======================
*/
void I_ShutdownSound(void) {
	int done = 0;
	int i;

	if ( S_initialized ) {
		// Stop all sounds, but don't destroy the FAudio buffers.
		for ( i = 0; i < NUM_SOUNDBUFFERS; ++i ) {
			activeSound_t * sound = &activeSounds[i];

			if ( sound == NULL ) {
				continue;
			}

			I_StopSound( sound->id, 0 );

			if ( sound->m_pSourceVoice ) {
				FAudioSourceVoice_FlushSourceBuffers( sound->m_pSourceVoice );
			}
		}

		for (i=1 ; i<NUMSFX ; i++) {
			if ( S_sfx[i].data && !(S_sfx[i].link) ) {
				//Z_Free( S_sfx[i].data );
				free( S_sfx[i].data );
			}
		}
	}

	I_StopSong( 0 );

	S_initialized = 0;
}

/*
======================
I_InitSoundHardware

Called from the tech4x initialization code. Sets up Doom classic's
sound channels.
======================
*/
void I_InitSoundHardware( int numOutputChannels_, int channelMask ) {
	::numOutputChannels = numOutputChannels_;

	// Initialize the F3DAudio
	//  Speaker geometry configuration on the final mix, specifies assignment of channels
	//  to speaker positions, defined as per WAVEFORMATEXTENSIBLE.dwChannelMask
	//  SpeedOfSound - not used by doomclassic
	F3DAudioInitialize( channelMask, 340.29f, F3DAudioInstance );

	for ( int i = 0; i < NUM_SOUNDBUFFERS; ++i ) {
		// Initialize source voices
		I_InitSoundChannel( i, numOutputChannels );
	}

	I_InitMusic();

	soundHardwareInitialized = true;
}


/*
======================
I_ShutdownSoundHardware

Called from the tech4x shutdown code. Tears down Doom classic's
sound channels.
======================
*/
void I_ShutdownSoundHardware() {
	soundHardwareInitialized = false;

	I_ShutdownMusic();

	for ( int i = 0; i < NUM_SOUNDBUFFERS; ++i ) {
		activeSound_t * sound = &activeSounds[i];

		if ( sound == NULL ) {
			continue;
		}

		if ( sound->m_pSourceVoice ) {
			FAudioSourceVoice_Stop( sound->m_pSourceVoice, 0, FAUDIO_COMMIT_NOW );
			FAudioSourceVoice_FlushSourceBuffers( sound->m_pSourceVoice );
			FAudioVoice_DestroyVoice( sound->m_pSourceVoice );
			sound->m_pSourceVoice = NULL;
		}

		if ( sound->m_DSPSettings.pMatrixCoefficients ) {
			delete [] sound->m_DSPSettings.pMatrixCoefficients;
			sound->m_DSPSettings.pMatrixCoefficients = NULL;
		}
	}
}

/*
======================
I_InitSoundChannel
======================
*/
void I_InitSoundChannel( int channel, int numOutputChannels_ ) {
	activeSound_t	*soundchannel = &activeSounds[ channel ];

	F3DAUDIO_VECTOR ZeroVector = { 0.0f, 0.0f, 0.0f };

	// Set up emitter parameters
	soundchannel->m_Emitter.OrientFront.x         = 0.0f;
	soundchannel->m_Emitter.OrientFront.y         = 0.0f;
	soundchannel->m_Emitter.OrientFront.z         = 1.0f;
	soundchannel->m_Emitter.OrientTop.x           = 0.0f;
	soundchannel->m_Emitter.OrientTop.y           = 1.0f;
	soundchannel->m_Emitter.OrientTop.z           = 0.0f;
	soundchannel->m_Emitter.Position              = ZeroVector;
	soundchannel->m_Emitter.Velocity              = ZeroVector;
	soundchannel->m_Emitter.pCone                 = &(soundchannel->m_Cone);
	soundchannel->m_Emitter.pCone->InnerAngle     = 0.0f; // Setting the inner cone angles to F3DAUDIO_2PI and
	// outer cone other than 0 causes
	// the emitter to act like a point emitter using the
	// INNER cone settings only.
	soundchannel->m_Emitter.pCone->OuterAngle     = 0.0f; // Setting the outer cone angles to zero causes
	// the emitter to act like a point emitter using the
	// OUTER cone settings only.
	soundchannel->m_Emitter.pCone->InnerVolume    = 0.0f;
	soundchannel->m_Emitter.pCone->OuterVolume    = 1.0f;
	soundchannel->m_Emitter.pCone->InnerLPF       = 0.0f;
	soundchannel->m_Emitter.pCone->OuterLPF       = 1.0f;
	soundchannel->m_Emitter.pCone->InnerReverb    = 0.0f;
	soundchannel->m_Emitter.pCone->OuterReverb    = 1.0f;

	soundchannel->m_Emitter.ChannelCount          = 1;
	soundchannel->m_Emitter.ChannelRadius         = 0.0f;
	soundchannel->m_Emitter.pVolumeCurve          = NULL;
	soundchannel->m_Emitter.pLFECurve             = NULL;
	soundchannel->m_Emitter.pLPFDirectCurve       = NULL;
	soundchannel->m_Emitter.pLPFReverbCurve       = NULL;
	soundchannel->m_Emitter.pReverbCurve          = NULL;
	soundchannel->m_Emitter.CurveDistanceScaler   = 1200.0f;
	soundchannel->m_Emitter.DopplerScaler         = 1.0f;
	soundchannel->m_Emitter.pChannelAzimuths      = g_EmitterAzimuths;

	soundchannel->m_DSPSettings.SrcChannelCount     = 1;
	soundchannel->m_DSPSettings.DstChannelCount     = numOutputChannels_;
	soundchannel->m_DSPSettings.pMatrixCoefficients = new FLOAT[ numOutputChannels_ ];

	// Create Source voice
	FAudioWaveFormatEx voiceFormat = {0};
	voiceFormat.wFormatTag = FAUDIO_FORMAT_PCM;
	voiceFormat.nChannels = 1;
    voiceFormat.nSamplesPerSec = 11025;
    voiceFormat.nAvgBytesPerSec = 11025;
    voiceFormat.nBlockAlign = 1;
    voiceFormat.wBitsPerSample = 8;
    voiceFormat.cbSize = 0;

	FAudio_CreateSourceVoice( soundSystemLocal.hardware.GetFAudio(), &soundchannel->m_pSourceVoice, &voiceFormat, 0, FAUDIO_DEFAULT_FREQ_RATIO, NULL, NULL, NULL );
}

/*
======================
I_InitSound
======================
*/
void I_InitSound() {

	if (S_initialized == 0) {
		int i;

		F3DAUDIO_VECTOR ZeroVector = { 0.0f, 0.0f, 0.0f };

		// Set up listener parameters
		doom_Listener.OrientFront.x        = 0.0f;
		doom_Listener.OrientFront.y        = 0.0f;
		doom_Listener.OrientFront.z        = 1.0f;
		doom_Listener.OrientTop.x          = 0.0f;
		doom_Listener.OrientTop.y          = 1.0f;
		doom_Listener.OrientTop.z          = 0.0f;
		doom_Listener.Position             = ZeroVector;
		doom_Listener.Velocity             = ZeroVector;

		for (i=1 ; i<NUMSFX ; i++)
		{ 
			// Alias? Example is the chaingun sound linked to pistol.
			if (!S_sfx[i].link)
			{
				// Load data from WAD file.
				S_sfx[i].data = getsfx( S_sfx[i].name, &lengths[i] );
			}	
			else
			{
				// Previously loaded already?
				S_sfx[i].data = S_sfx[i].link->data;
				lengths[i] = lengths[(S_sfx[i].link - S_sfx)/sizeof(sfxinfo_t)];
			}
		}

		S_initialized = 1;
	}
}

/*
======================
I_SubmitSound
======================
*/
void I_SubmitSound(void)
{
	// Only do this for player 0, it will still handle positioning
	//		for other players, but it can't be outside the game 
	//		frame like the soundEvents are.
	if ( DoomLib::GetPlayer() == 0 ) {
		// Do 3D positioning of sounds
		I_UpdateSound();

		// Check for XMP notifications
		I_UpdateMusic();
	}
}


// =========================================================
// =========================================================
// Background Music
// =========================================================
// =========================================================

/*
======================
I_SetMusicVolume
======================
*/
void I_SetMusicVolume(int volume)
{
	x_MusicVolume = (float)volume / 15.f;
}

/*
======================
I_InitMusic
======================
*/
void I_InitMusic(void)		
{
	if ( !Music_initialized ) {
		// Initialize Timidity
		Timidity_Init( MIDI_RATE, MIDI_FORMAT, MIDI_CHANNELS, MIDI_RATE, "classicmusic/gravis.cfg" );

		musicBuffer = NULL;
		totalBufferSize = 0;
		waitingForMusic = false;
		musicReady = false;

		// Create Source voice
		FAudioWaveFormatEx voiceFormat = {0};
		voiceFormat.wFormatTag = FAUDIO_FORMAT_PCM;
		voiceFormat.nChannels = 2;
		voiceFormat.nSamplesPerSec = MIDI_RATE;
		voiceFormat.nAvgBytesPerSec = MIDI_RATE * MIDI_FORMAT_BYTES * 2;
		voiceFormat.nBlockAlign = MIDI_FORMAT_BYTES * 2;
		voiceFormat.wBitsPerSample = MIDI_FORMAT_BYTES * 8;
		voiceFormat.cbSize = 0;

		FAudio_CreateSourceVoice( soundSystemLocal.hardware.GetFAudio(), &pMusicSourceVoice, &voiceFormat, FAUDIO_VOICE_MUSIC, FAUDIO_DEFAULT_FREQ_RATIO, NULL, NULL, NULL );

		Music_initialized = true;
	}
}

/*
======================
I_ShutdownMusic
======================
*/
void I_ShutdownMusic(void)	
{
	I_StopSong( 0 );

	if ( Music_initialized ) {
		if ( pMusicSourceVoice ) {
			FAudioSourceVoice_Stop( pMusicSourceVoice, 0, FAUDIO_COMMIT_NOW );
			FAudioSourceVoice_FlushSourceBuffers( pMusicSourceVoice );
			FAudioVoice_DestroyVoice( pMusicSourceVoice );
			pMusicSourceVoice = NULL;
		}

		if ( musicBuffer ) {
			free( musicBuffer );
		}

		Timidity_Shutdown();
	}

	pMusicSourceVoice = NULL;
	musicBuffer = NULL;

	totalBufferSize = 0;
	waitingForMusic = false;
	musicReady = false;

	Music_initialized = false;
}

int Mus2Midi(unsigned char* bytes, unsigned char* out, int* len);

namespace {
	const int MaxMidiConversionSize = 1024 * 1024;
	unsigned char midiConversionBuffer[MaxMidiConversionSize];
}

/*
======================
I_LoadSong
======================
*/
void I_LoadSong( const char * songname ) {
	idStr lumpName = "d_";
	lumpName += static_cast< const char * >( songname );

	unsigned char * musFile = static_cast< unsigned char * >( W_CacheLumpName( lumpName.c_str(), PU_STATIC_SHARED ) );

	int length = 0;
	Mus2Midi( musFile, midiConversionBuffer, &length );

	doomMusic = Timidity_LoadSongMem( midiConversionBuffer, length );

	if ( doomMusic ) {
		musicBuffer = (byte *)malloc( MIDI_CHANNELS * MIDI_FORMAT_BYTES * doomMusic->samples );
		totalBufferSize = doomMusic->samples * MIDI_CHANNELS * MIDI_FORMAT_BYTES;

		Timidity_Start( doomMusic );

		int		rc = RC_NO_RETURN_VALUE;
		int		num_bytes = 0;
		int		offset = 0;

		do {
			rc = Timidity_PlaySome( musicBuffer + offset, MIDI_RATE, &num_bytes );
			offset += num_bytes;
		} while ( rc != RC_TUNE_END );

		Timidity_Stop();
		Timidity_FreeSong( doomMusic );
	}

	musicReady = true;
}

/*
======================
I_PlaySong
======================
*/
void I_PlaySong( const char *songname, int looping)
{
	if ( !Music_initialized ) {
		return;
	}

	if ( pMusicSourceVoice != NULL ) {
		// Stop the voice and flush packets before freeing the musicBuffer
		FAudioSourceVoice_Stop( pMusicSourceVoice, 0, FAUDIO_COMMIT_NOW );
		FAudioSourceVoice_FlushSourceBuffers( pMusicSourceVoice );
	}

	// Make sure voice is stopped before we free the buffer
	bool isStopped = false;
	int d = 0;
	while ( !isStopped ) {
		FAudioVoiceState test;

		if ( pMusicSourceVoice != NULL ) {
			FAudioSourceVoice_GetState( pMusicSourceVoice, &test, 0 );
		}

		if ( test.pCurrentBufferContext == NULL && test.BuffersQueued == 0 ) {
			isStopped = true;
		}
		//I_Printf( "waiting to stop (%d)\n", d++ );
	}

	// Clear old state
	if ( musicBuffer != NULL ) {
		free( musicBuffer );
		musicBuffer = NULL;
	}

	musicReady = false;
	I_LoadSong( songname );
	waitingForMusic = true;

	if ( DoomLib::GetPlayer() >= 0 ) {
		::g->mus_looping = looping;
	}
}

/*
======================
I_UpdateMusic
======================
*/
void I_UpdateMusic( void ) {
	if ( !Music_initialized ) {
		return;
	}

	if ( waitingForMusic ) {

		if ( musicReady && pMusicSourceVoice != NULL ) {

			if ( musicBuffer ) {
				// Set up packet
				FAudioBuffer Packet = { 0 };
				Packet.Flags = FAUDIO_END_OF_STREAM;
				Packet.AudioBytes = totalBufferSize;
				Packet.pAudioData = (unsigned char*)musicBuffer;
				Packet.PlayBegin = 0;
				Packet.PlayLength = 0;
				Packet.LoopBegin = 0;
				Packet.LoopLength = 0;
				Packet.LoopCount = ::g->mus_looping ? FAUDIO_LOOP_INFINITE : 0;
				Packet.pContext = NULL;

				// Submit packet
				FAudioSourceVoice_SubmitSourceBuffer( pMusicSourceVoice, &Packet, NULL );

				// Play the source voice
				FAudioSourceVoice_Start( pMusicSourceVoice, 0, FAUDIO_COMMIT_NOW );
			}

			waitingForMusic = false;
		}
	}

	if ( pMusicSourceVoice != NULL ) {
		// Set the volume
		FAudioVoice_SetVolume( pMusicSourceVoice, x_MusicVolume * GLOBAL_VOLUME_MULTIPLIER, FAUDIO_COMMIT_NOW );
	}
}

/*
======================
I_PauseSong
======================
*/
void I_PauseSong (int handle)
{
	if ( !Music_initialized ) {
		return;
	}

	if ( pMusicSourceVoice != NULL ) {
		// Stop the music source voice
		FAudioSourceVoice_Stop( pMusicSourceVoice, 0, FAUDIO_COMMIT_NOW );
	}
}

/*
======================
I_ResumeSong
======================
*/
void I_ResumeSong (int handle)
{
	if ( !Music_initialized ) {
		return;
	}

	// Stop the music source voice
	if ( pMusicSourceVoice != NULL ) {
		FAudioSourceVoice_Start( pMusicSourceVoice, 0, FAUDIO_COMMIT_NOW );
	}
}

/*
======================
I_StopSong
======================
*/
void I_StopSong(int handle)
{
	if ( !Music_initialized ) {
		return;
	}

	// Stop the music source voice
	if ( pMusicSourceVoice != NULL ) {
		FAudioSourceVoice_Stop( pMusicSourceVoice, 0, FAUDIO_COMMIT_NOW );
	}
}

/*
======================
I_UnRegisterSong
======================
*/
void I_UnRegisterSong(int handle)
{
	// does nothing
}

/*
======================
I_RegisterSong
======================
*/
int I_RegisterSong(void* data, int length)
{
	// does nothing
	return 0;
}


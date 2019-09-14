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
#include "../snd_local.h"

idCVar s_skipHardwareSets( "s_skipHardwareSets", "0", CVAR_BOOL, "Do all calculation, but skip FAudio calls" );
idCVar s_debugHardware( "s_debugHardware", "0", CVAR_BOOL, "Print a message any time a hardware voice changes" );

// The whole system runs at this sample rate
static int SYSTEM_SAMPLE_RATE = 44100;
static float ONE_OVER_SYSTEM_SAMPLE_RATE = 1.0f / SYSTEM_SAMPLE_RATE;

static FAudioVoiceCallback streamContext;

/*
========================
OnBufferEnd
========================
*/
void FAUDIOCALL OnBufferEnd( FAudioVoiceCallback * callback, void * pContext ) {
	idSoundSystemLocal::bufferContext_t * bufferContext = (idSoundSystemLocal::bufferContext_t *) pContext;
	soundSystemLocal.ReleaseStreamBufferContext( bufferContext );
}

/*
========================
OnBufferStart
========================
*/
void FAUDIOCALL OnBufferStart( FAudioVoiceCallback * callback, void * pContext ) {
	idSoundSystemLocal::bufferContext_t * bufferContext = (idSoundSystemLocal::bufferContext_t *) pContext;
	bufferContext->voice->OnBufferStart( bufferContext->sample, bufferContext->bufferNumber );
}

/*
========================
OnVoiceError
========================
*/
void FAUDIOCALL OnVoiceError( FAudioVoiceCallback * callback, void * pContext, uint32_t hr ) {
	idLib::Warning( "OnVoiceError( %d )", hr );
}

/*
========================
idSoundVoice_FAudio::idSoundVoice_FAudio
========================
*/
idSoundVoice_FAudio::idSoundVoice_FAudio()
:	pSourceVoice( NULL ),
	leadinSample( NULL ),
	loopingSample( NULL ),
	formatTag( 0 ),
	numChannels( 0 ),
	sampleRate( 0 ),
	paused( true ),
	hasVUMeter( false ) {

}

/*
========================
idSoundVoice_FAudio::~idSoundVoice_FAudio
========================
*/
idSoundVoice_FAudio::~idSoundVoice_FAudio() {
	DestroyInternal();
}

/*
========================
idSoundVoice_FAudio::CompatibleFormat
========================
*/
bool idSoundVoice_FAudio::CompatibleFormat( idSoundSample_FAudio * s ) {
	if ( pSourceVoice == NULL ) {
		// If this voice has never been allocated, then it's compatible with everything
		return true;
	}
	return false;
}

/*
========================
idSoundVoice_FAudio::Create
========================
*/
void idSoundVoice_FAudio::Create( const idSoundSample * leadinSample_, const idSoundSample * loopingSample_ ) {
	if ( IsPlaying() ) {
		// This should never hit
		Stop();
		return;
	}
	leadinSample = (idSoundSample_FAudio *)leadinSample_;
	loopingSample = (idSoundSample_FAudio *)loopingSample_;

	if ( pSourceVoice != NULL && CompatibleFormat( leadinSample ) ) {
		sampleRate = leadinSample->format.basic.samplesPerSec;
	} else {
		DestroyInternal();
		formatTag = leadinSample->format.basic.formatTag;
		numChannels = leadinSample->format.basic.numChannels;
		sampleRate = leadinSample->format.basic.samplesPerSec;

		streamContext.OnBufferEnd = ::OnBufferEnd;
		streamContext.OnBufferStart = ::OnBufferStart;
		streamContext.OnVoiceError = ::OnVoiceError;
		FAudio_CreateSourceVoice( soundSystemLocal.hardware.pFAudio, &pSourceVoice, (const FAudioWaveFormatEx *)&leadinSample->format, FAUDIO_VOICE_USEFILTER, 4.0f, &streamContext, NULL, NULL );
		if ( pSourceVoice == NULL ) {
			// If this hits, then we are most likely passing an invalid sample format, which should have been caught by the loader (and the sample defaulted)
			return;
		}
		if ( s_debugHardware.GetBool() ) {
			if ( loopingSample == NULL || loopingSample == leadinSample ) {
				idLib::Printf( "%dms: %p created for %s\n", Sys_Milliseconds(), pSourceVoice, leadinSample ? leadinSample->GetName() : "<null>" );
			} else {
				idLib::Printf( "%dms: %p created for %s and %s\n", Sys_Milliseconds(), pSourceVoice, leadinSample ? leadinSample->GetName() : "<null>", loopingSample ? loopingSample->GetName() : "<null>" );
			}
		}
	}
	sourceVoiceRate = sampleRate;
	FAudioSourceVoice_SetSourceSampleRate( pSourceVoice, sampleRate );
	FAudioVoice_SetVolume( pSourceVoice, 0.0f, FAUDIO_COMMIT_NOW );
}

/*
========================
idSoundVoice_FAudio::DestroyInternal
========================
*/
void idSoundVoice_FAudio::DestroyInternal() {
	if ( pSourceVoice != NULL ) {
		if ( s_debugHardware.GetBool() ) {
			idLib::Printf( "%dms: %p destroyed\n", Sys_Milliseconds(), pSourceVoice );
		}
		FAudioVoice_DestroyVoice( pSourceVoice );
		pSourceVoice = NULL;
		hasVUMeter = false;
	}
}

/*
========================
idSoundVoice_FAudio::Start
========================
*/
void idSoundVoice_FAudio::Start( int offsetMS, int ssFlags ) {

	if ( s_debugHardware.GetBool() ) {
		idLib::Printf( "%dms: %p starting %s @ %dms\n", Sys_Milliseconds(), pSourceVoice, leadinSample ? leadinSample->GetName() : "<null>", offsetMS );
	}

	if ( !leadinSample ) {
		return;
	}
	if ( !pSourceVoice ) {
		return;
	}

	if ( leadinSample->IsDefault() ) {
		idLib::Warning( "Starting defaulted sound sample %s", leadinSample->GetName() );
	}

	bool flicker = ( ssFlags & SSF_NO_FLICKER ) == 0;

	if ( flicker != hasVUMeter ) {
		hasVUMeter = flicker;

		if ( flicker ) {
			FAPO * vuMeter = NULL;
			if ( FAudioCreateVolumeMeter( &vuMeter, 0 ) == 0 ) {

				FAudioEffectDescriptor descriptor;
				descriptor.InitialState = true;
				descriptor.OutputChannels = leadinSample->NumChannels();
				descriptor.pEffect = vuMeter;

				FAudioEffectChain chain;
				chain.EffectCount = 1;
				chain.pEffectDescriptors = &descriptor;

				FAudioVoice_SetEffectChain( pSourceVoice, &chain );

				vuMeter->Release(vuMeter);
			}
		} else {
			FAudioVoice_SetEffectChain( pSourceVoice, NULL );
		}
	}

	assert( offsetMS >= 0 );
	int offsetSamples = MsecToSamples( offsetMS, leadinSample->SampleRate() );
	if ( loopingSample == NULL && offsetSamples >= leadinSample->playLength ) {
		return;
	}

	RestartAt( offsetSamples );
	Update();
	UnPause();
}

/*
========================
idSoundVoice_FAudio::RestartAt
========================
*/
int idSoundVoice_FAudio::RestartAt( int offsetSamples ) {
	offsetSamples &= ~127;

	idSoundSample_FAudio * sample = leadinSample;
	if ( offsetSamples >= leadinSample->playLength ) {
		if ( loopingSample != NULL ) {
			offsetSamples %= loopingSample->playLength;
			sample = loopingSample;
		} else {
			return 0;
		}
	}

	int previousNumSamples = 0;
	for ( int i = 0; i < sample->buffers.Num(); i++ ) {
		if ( sample->buffers[i].numSamples > sample->playBegin + offsetSamples ) {
			return SubmitBuffer( sample, i, sample->playBegin + offsetSamples - previousNumSamples );
		}
		previousNumSamples = sample->buffers[i].numSamples;
	}

	return 0;
}

/*
========================
idSoundVoice_FAudio::SubmitBuffer
======================== 
*/
int idSoundVoice_FAudio::SubmitBuffer( idSoundSample_FAudio * sample, int bufferNumber, int offset ) {

	if ( sample == NULL || ( bufferNumber < 0 ) || ( bufferNumber >= sample->buffers.Num() ) ) {
		return 0;
	}
	idSoundSystemLocal::bufferContext_t * bufferContext = soundSystemLocal.ObtainStreamBufferContext();
	if ( bufferContext == NULL ) {
		idLib::Warning( "No free buffer contexts!" );
		return 0;
	}

	bufferContext->voice = this;
	bufferContext->sample = sample;
	bufferContext->bufferNumber = bufferNumber;

	FAudioBuffer buffer = { 0 };
	if ( offset > 0 ) {
		int previousNumSamples = 0;
		if ( bufferNumber > 0 ) {
			previousNumSamples = sample->buffers[bufferNumber-1].numSamples;
		}
		buffer.PlayBegin = offset;
		buffer.PlayLength = sample->buffers[bufferNumber].numSamples - previousNumSamples - offset;
	}
	buffer.AudioBytes = sample->buffers[bufferNumber].bufferSize;
	buffer.pAudioData = (uint8 *)sample->buffers[bufferNumber].buffer;
	buffer.pContext = bufferContext;
	if ( ( loopingSample == NULL ) && ( bufferNumber == sample->buffers.Num() - 1 ) ) {
		buffer.Flags = FAUDIO_END_OF_STREAM;
	}
	FAudioSourceVoice_SubmitSourceBuffer( pSourceVoice, &buffer, NULL );

	return buffer.AudioBytes;
}

/*
========================
idSoundVoice_FAudio::Update
========================
*/
bool idSoundVoice_FAudio::Update() {
	if ( pSourceVoice == NULL || leadinSample == NULL ) {
		return false;
	}

	FAudioVoiceState state;
	FAudioSourceVoice_GetState( pSourceVoice, &state, 0 );

	const int srcChannels = leadinSample->NumChannels();

	float pLevelMatrix[ MAX_CHANNELS_PER_VOICE * MAX_CHANNELS_PER_VOICE ] = { 0 };
	CalculateSurround( srcChannels, pLevelMatrix, 1.0f );

	if ( s_skipHardwareSets.GetBool() ) {
		return true;
	}

	FAudioVoice_SetOutputMatrix( pSourceVoice, soundSystemLocal.hardware.pMasterVoice, srcChannels, dstChannels, pLevelMatrix, OPERATION_SET );

	assert( idMath::Fabs( gain ) <= FAUDIO_MAX_VOLUME_LEVEL );
	FAudioVoice_SetVolume( pSourceVoice, gain, OPERATION_SET );

	SetSampleRate( sampleRate, OPERATION_SET );

	// we don't do this any longer because we pause and unpause explicitly when the soundworld is paused or unpaused
	// UnPause();
	return true;
}

/*
========================
idSoundVoice_FAudio::IsPlaying
========================
*/
bool idSoundVoice_FAudio::IsPlaying() {
	if ( pSourceVoice == NULL ) {
		return false;
	}
	FAudioVoiceState state;
	FAudioSourceVoice_GetState( pSourceVoice, &state, 0 );
	return ( state.BuffersQueued != 0 );
}

/*
========================
idSoundVoice_FAudio::FlushSourceBuffers
========================
*/
void idSoundVoice_FAudio::FlushSourceBuffers() {
	if ( pSourceVoice != NULL ) {
		FAudioSourceVoice_FlushSourceBuffers( pSourceVoice );
	}
}

/*
========================
idSoundVoice_FAudio::Pause
========================
*/
void idSoundVoice_FAudio::Pause() {
	if ( !pSourceVoice || paused ) {
		return;
	}
	if ( s_debugHardware.GetBool() ) {
		idLib::Printf( "%dms: %p pausing %s\n", Sys_Milliseconds(), pSourceVoice, leadinSample ? leadinSample->GetName() : "<null>" );
	}
	FAudioSourceVoice_Stop( pSourceVoice, 0, OPERATION_SET );
	paused = true;
}

/*
========================
idSoundVoice_FAudio::UnPause
========================
*/
void idSoundVoice_FAudio::UnPause() {
	if ( !pSourceVoice || !paused ) {
		return;
	}
	if ( s_debugHardware.GetBool() ) {
		idLib::Printf( "%dms: %p unpausing %s\n", Sys_Milliseconds(), pSourceVoice, leadinSample ? leadinSample->GetName() : "<null>" );
	}
	FAudioSourceVoice_Start( pSourceVoice, 0, OPERATION_SET );
	paused = false;
}

/*
========================
idSoundVoice_FAudio::Stop
========================
*/
void idSoundVoice_FAudio::Stop() {
	if ( !pSourceVoice ) {
		return;
	}
	if ( !paused ) {
		if ( s_debugHardware.GetBool() ) {
			idLib::Printf( "%dms: %p stopping %s\n", Sys_Milliseconds(), pSourceVoice, leadinSample ? leadinSample->GetName() : "<null>" );
		}
		FAudioSourceVoice_Stop( pSourceVoice, 0, OPERATION_SET );
		paused = true;
	}
}

/*
========================
idSoundVoice_FAudio::GetAmplitude
========================
*/
float idSoundVoice_FAudio::GetAmplitude() {
	if ( !hasVUMeter ) {
		return 1.0f;
	}

	float peakLevels[ MAX_CHANNELS_PER_VOICE ];
	float rmsLevels[ MAX_CHANNELS_PER_VOICE ];

	FAudioFXVolumeMeterLevels levels;
	levels.ChannelCount = leadinSample->NumChannels();
	levels.pPeakLevels = peakLevels;
	levels.pRMSLevels = rmsLevels;

	if ( levels.ChannelCount > MAX_CHANNELS_PER_VOICE ) {
		levels.ChannelCount = MAX_CHANNELS_PER_VOICE;
	}

	if ( FAudioVoice_GetEffectParameters( pSourceVoice, 0, &levels, sizeof( levels ) ) != 0 ) {
		return 0.0f;
	}

	if ( levels.ChannelCount == 1 ) {
		return rmsLevels[0];
	}

	float rms = 0.0f;
	for ( uint32 i = 0; i < levels.ChannelCount; i++ ) {
		rms += rmsLevels[i];
	}

	return rms / (float)levels.ChannelCount;
}

/*
========================
idSoundVoice_FAudio::ResetSampleRate
========================
*/
void idSoundVoice_FAudio::SetSampleRate( uint32 newSampleRate, uint32 operationSet ){
	if ( pSourceVoice == NULL || leadinSample == NULL ) {
		return;
	}

	sampleRate = newSampleRate;

	FAudioFilterParameters filter;
	filter.Type = FAudioLowPassFilter;
	filter.OneOverQ = 1.0f;			// [0.0f, FAUDIO_MAX_FILTER_ONEOVERQ]
	float cutoffFrequency = 1000.0f / Max( 0.01f, occlusion );
	if ( cutoffFrequency * 6.0f >= (float)sampleRate ) {
		filter.Frequency = FAUDIO_MAX_FILTER_FREQUENCY;
	} else {
		filter.Frequency = 2.0f * idMath::Sin( idMath::PI * cutoffFrequency / (float)sampleRate );
	}
	assert( filter.Frequency >= 0.0f && filter.Frequency <= FAUDIO_MAX_FILTER_FREQUENCY );
	filter.Frequency = idMath::ClampFloat( 0.0f, FAUDIO_MAX_FILTER_FREQUENCY, filter.Frequency );

	FAudioVoice_SetFilterParameters( pSourceVoice, &filter, operationSet );

	float freqRatio = pitch * (float)sampleRate / (float)sourceVoiceRate;
	assert( freqRatio >= FAUDIO_MIN_FREQ_RATIO && freqRatio <= FAUDIO_MAX_FREQ_RATIO );
	freqRatio = idMath::ClampFloat( FAUDIO_MIN_FREQ_RATIO, FAUDIO_MAX_FREQ_RATIO, freqRatio );

	// if the value specified for maxFreqRatio is too high for the specified format, the call to CreateSourceVoice will fail
	FAudioSourceVoice_SetFrequencyRatio( pSourceVoice, freqRatio, operationSet );
}

/*
========================
idSoundVoice_FAudio::OnBufferStart
========================
*/
void idSoundVoice_FAudio::OnBufferStart( idSoundSample_FAudio * sample, int bufferNumber ) {
	SetSampleRate( sample->SampleRate(), FAUDIO_COMMIT_NOW );

	idSoundSample_FAudio * nextSample = sample;
	int nextBuffer = bufferNumber + 1;
	if ( nextBuffer == sample->buffers.Num() ) {
		if ( sample == leadinSample ) {
			if ( loopingSample == NULL ) {
				return;
			}
			nextSample = loopingSample;
		}
		nextBuffer = 0;
	}

	SubmitBuffer( nextSample, nextBuffer, 0 );
}

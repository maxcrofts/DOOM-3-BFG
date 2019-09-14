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
#ifndef __FA_SOUNDHARDWARE_H__
#define __FA_SOUNDHARDWARE_H__

class idSoundSample_FAudio;
class idSoundVoice_FAudio;

/*
================================================
idSoundHardware_FAudio
================================================
*/
class idSoundHardware_FAudio {
public:
					idSoundHardware_FAudio();

	void			Init();
	void			Shutdown();

	void 			Update();

	idSoundVoice *	AllocateVoice( const idSoundSample * leadinSample, const idSoundSample * loopingSample );
	void			FreeVoice( idSoundVoice * voice );

	// video playback needs this
	FAudio *		GetFAudio() const { return pFAudio; };

	int				GetNumZombieVoices() const { return zombieVoices.Num(); }
	int				GetNumFreeVoices() const { return freeVoices.Num(); }

protected:
	friend class idSoundSample_FAudio;
	friend class idSoundVoice_FAudio;

private:
	FAudio * pFAudio;
	FAudioMasteringVoice * pMasterVoice;
	FAudioSubmixVoice * pSubmixVoice;

	FAudioEngineCallback	soundEngineCallback;
	int					lastResetTime;

	int					outputChannels;
	int					channelMask;

	idDebugGraph *		vuMeterRMS;
	idDebugGraph *		vuMeterPeak;
	int					vuMeterPeakTimes[ 8 ];

	// Can't stop and start a voice on the same frame, so we have to double this to handle the worst case scenario of stopping all voices and starting a full new set
	idStaticList<idSoundVoice_FAudio, MAX_HARDWARE_VOICES * 2 > voices;
	idStaticList<idSoundVoice_FAudio *, MAX_HARDWARE_VOICES * 2 > zombieVoices;
	idStaticList<idSoundVoice_FAudio *, MAX_HARDWARE_VOICES * 2 > freeVoices;
};

/*
================================================
idSoundHardware
================================================
*/
class idSoundHardware : public idSoundHardware_FAudio {
};

#endif

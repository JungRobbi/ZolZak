#pragma once
#include "stdafx.h"
#include <fmod.h>

#define MAX_VOLUME 1.0f
#define MIN_VOLUME 0.0f

class Sound
{
private:
	static FMOD_SYSTEM* FMODSystem;

	FMOD_SOUND* m_Sound;
	FMOD_CHANNEL* m_Channel;

	FMOD_DSP* Lowpass;
	FMOD_DSP* Highpass;
	FMOD_DSP* Tremolo;
	float Volume = 0.3;

public:
	Sound(char* SoundFilePath, bool IsLoop);
	~Sound();

	static void InitFmodSystem();
	static void ReleaseFmodSystem();
	static void SystemUpdate();
	void Play();
	void Stop();
	
	void AddDsp();
	void RemoveDsp();

	void VolumeUp();
	void VolumeDown();

	FMOD_BOOL CheckEndSound();
};
#pragma once
#include "stdafx.h"
#include <fmod.h>

#define MAX_VOLUME 1.0f
#define MIN_VOLUME 0.0f

class Sound
{
private:
	static FMOD_SYSTEM* FMODSystem;
	static XMFLOAT3* PlayerPosition;
	FMOD_SOUND* m_Sound;
	FMOD_CHANNEL* m_Channel;

	FMOD_DSP* Lowpass;
	FMOD_DSP* Highpass;
	FMOD_DSP* Tremolo;
	float Volume = 1.0;
	float AttVolume = 1.0f;

public:
	Sound(char* SoundFilePath, FMOD_MODE Mode, XMFLOAT3* SoundPos);
	~Sound();

	static void InitFmodSystem();
	static void ReleaseFmodSystem();
	static void SystemUpdate(XMFLOAT3* PlayerPos, XMFLOAT3* PlayerForward, XMFLOAT3* PlayerUp);
	void Play();
	void Stop();
	void Replay();

	void AddDsp();
	void RemoveDsp();

	void VolumeUp();
	void VolumeDown();
	void SetVolume(float v);

	FMOD_BOOL CheckEndSound();
};
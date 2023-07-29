#include "Sound.h"

FMOD_SYSTEM* Sound::FMODSystem;
Sound::Sound(char* SoundFilePath, bool IsLoop)
{
	if (IsLoop)
	{
		FMOD_System_CreateSound(FMODSystem, SoundFilePath, FMOD_LOOP_NORMAL, 0, &m_Sound);
	}
	else
	{
		FMOD_System_CreateSound(FMODSystem, SoundFilePath, FMOD_DEFAULT, 0, &m_Sound);
	}

	FMOD_System_CreateDSPByType(FMODSystem, FMOD_DSP_TYPE_LOWPASS, &Lowpass);
	FMOD_DSP_SetParameterFloat(Lowpass, FMOD_DSP_LOWPASS_CUTOFF, 550.0f);
	FMOD_System_CreateDSPByType(FMODSystem, FMOD_DSP_TYPE_HIGHPASS, &Highpass);
	FMOD_DSP_SetParameterFloat(Highpass, FMOD_DSP_HIGHPASS_CUTOFF, 20.0f);
	FMOD_System_CreateDSPByType(FMODSystem, FMOD_DSP_TYPE_TREMOLO, &Tremolo);
	FMOD_DSP_SetParameterFloat(Tremolo, 0, 1.0f);

	Play();
}
Sound::~Sound()
{
	Stop();
	FMOD_Sound_Release(m_Sound);
}

void Sound::InitFmodSystem()
{
	FMOD_System_Create(&FMODSystem, FMOD_VERSION);
	FMOD_System_Init(FMODSystem, 32, FMOD_INIT_NORMAL, NULL);
}

void Sound::ReleaseFmodSystem()
{
	FMOD_System_Close(FMODSystem);
	FMOD_System_Release(FMODSystem);
}

void Sound::SystemUpdate()
{
	FMOD_System_Update(FMODSystem);
}

void Sound::Play()
{
	FMOD_System_PlaySound(FMODSystem, m_Sound, NULL, false, &m_Channel);
	FMOD_Channel_SetVolume(m_Channel, Volume);
}

void Sound::Stop()
{
	FMOD_Channel_Stop(m_Channel);
}

void Sound::Replay()
{
	FMOD_Channel_SetPaused(m_Channel, true);
	FMOD_Channel_SetPosition(m_Channel, 0, FMOD_TIMEUNIT_MS);
	FMOD_Channel_SetPaused(m_Channel, false);
}

void Sound::AddDsp()
{
	FMOD_Channel_AddDSP(m_Channel, 0, Lowpass);
	//FMOD_Channel_AddDSP(m_Channel, 0, Tremolo);
	FMOD_Channel_AddDSP(m_Channel, 0, Highpass);
}

void Sound::RemoveDsp()
{
	FMOD_Channel_RemoveDSP(m_Channel, Lowpass);
	//FMOD_Channel_RemoveDSP(m_Channel, Tremolo);
	FMOD_Channel_RemoveDSP(m_Channel, Highpass);
}

void Sound::VolumeUp()
{
	if (Volume < MAX_VOLUME)
	{
		Volume += 0.1f;
	}
	FMOD_Channel_SetVolume(m_Channel, Volume);
}

void Sound::VolumeDown()
{
	if (Volume > MIN_VOLUME)
	{
		Volume -= 0.1f;
	}
	FMOD_Channel_SetVolume(m_Channel, Volume);
}

FMOD_BOOL Sound::CheckEndSound()
{
	FMOD_BOOL IsPlaying;
	FMOD_Channel_IsPlaying(m_Channel, &IsPlaying);
	return IsPlaying;
}

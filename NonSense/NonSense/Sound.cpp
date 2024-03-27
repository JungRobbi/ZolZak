#include "Sound.h"
#include "GameFramework.h"

FMOD_SYSTEM* Sound::FMODSystem;
XMFLOAT3* Sound::PlayerPosition;

Sound::Sound(char* SoundFilePath, FMOD_MODE Mode, XMFLOAT3* SoundPos)
{
	FMOD_System_CreateSound(FMODSystem, SoundFilePath, Mode, 0, &m_Sound);
	FMOD_System_CreateDSPByType(FMODSystem, FMOD_DSP_TYPE_LOWPASS, &Lowpass);
	FMOD_DSP_SetParameterFloat(Lowpass, FMOD_DSP_LOWPASS_CUTOFF, 550.0f);
	FMOD_System_CreateDSPByType(FMODSystem, FMOD_DSP_TYPE_HIGHPASS, &Highpass);
	FMOD_DSP_SetParameterFloat(Highpass, FMOD_DSP_HIGHPASS_CUTOFF, 20.0f);
	FMOD_System_CreateDSPByType(FMODSystem, FMOD_DSP_TYPE_TREMOLO, &Tremolo);
	FMOD_DSP_SetParameterFloat(Tremolo, 0, 1.0f);
	SoundPosition = *SoundPos;
	SoundMode = Mode;
	Play();
	if (Mode & FMOD_3D_WORLDRELATIVE)
	{
		XMFLOAT3 RelativePos = Vector3::Subtract(*SoundPos, GameFramework::MainGameFramework->m_pPlayer->GetPosition());
		FMOD_VECTOR vec = { SoundPos->x,SoundPos->y ,SoundPos->z };
		FMOD_VECTOR vel = { 0,0,0 };
		FMOD_Channel_Set3DAttributes(m_Channel, &vec, &vel);
		FMOD_Channel_Set3DMinMaxDistance(m_Channel, 1.0f, 100.0f);
		FMOD_Channel_Set3DLevel(m_Channel, 0.8f);

		float len = Vector3::Length(RelativePos);
		if (len > 50.f)
		{
			FMOD_Channel_SetVolume(m_Channel, 0.0f);
		}
		else
		{
			AttVolume = 50.0f / (50.0f + len);
			FMOD_Channel_SetVolume(m_Channel, Volume * AttVolume);
		}
	}
	if (Mode & FMOD_3D_HEADRELATIVE)
	{
		XMFLOAT3 RelativePos = Vector3::Subtract(*SoundPos, GameFramework::MainGameFramework->m_pPlayer->GetPosition());
		FMOD_VECTOR RelativeVec = { RelativePos.x,RelativePos.y,RelativePos.z };
		FMOD_VECTOR vel = { 0,0,0 };
		FMOD_Channel_Set3DAttributes(m_Channel, &RelativeVec, &vel);
		FMOD_Channel_Set3DMinMaxDistance(m_Channel, 1.0f, 100.0f);
		FMOD_Channel_Set3DLevel(m_Channel, 0.8f);

		float len = Vector3::Length(RelativePos);
		if (len > 50.f)
		{
			FMOD_Channel_SetVolume(m_Channel, 0.0f);
			std::cout << "out of range" << std::endl;
		}
		else
		{
			AttVolume = 50.0f / (50.0f + len);
			FMOD_Channel_SetVolume(m_Channel, Volume * AttVolume);
		}
	}
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
	//FMOD_System_Set3DSettings(FMODSystem, 1.0f, 1.0f, FMOD_3D_LINEARSQUAREROLLOFF);
}

void Sound::ReleaseFmodSystem()
{
	FMOD_System_Close(FMODSystem);
	FMOD_System_Release(FMODSystem);
}

void Sound::SystemUpdate(XMFLOAT3* PlayerPos, XMFLOAT3* PlayerForward, XMFLOAT3* PlayerUp)
{
	PlayerPosition = PlayerPos;
	FMOD_VECTOR pos = { PlayerPos->x,PlayerPos->y,PlayerPos->z };
	FMOD_VECTOR Forward = { PlayerForward->x,PlayerForward->y,PlayerForward->z };
	FMOD_VECTOR Up = { PlayerUp->x,PlayerUp->y,PlayerUp->z };
	FMOD_VECTOR vel = { 0,0,0 };

	FMOD_System_Set3DListenerAttributes(FMODSystem, 0, &pos, &vel, &Forward, &Up);
	FMOD_System_Update(FMODSystem);

}

void Sound::Play()
{
	FMOD_System_PlaySound(FMODSystem, m_Sound, NULL, false, &m_Channel);
	FMOD_Channel_SetVolume(m_Channel, Volume * AttVolume);
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

void Sound::Update()
{
	if (SoundMode & FMOD_3D_WORLDRELATIVE)
	{
		XMFLOAT3 RelativePos = Vector3::Subtract(SoundPosition, GameFramework::MainGameFramework->m_pPlayer->GetPosition());
		float len = Vector3::Length(RelativePos);
		if (len > 50.f)
		{
			FMOD_Channel_SetVolume(m_Channel, 0.0f);
		}
		else
		{
			AttVolume = 50.0f / (50.0f + len);
			FMOD_Channel_SetVolume(m_Channel, Volume * AttVolume);
		}
	}
}

void Sound::VolumeUp()
{
	if (Volume < MAX_VOLUME)
	{
		Volume += 0.1f;
	}
	FMOD_Channel_SetVolume(m_Channel, Volume * AttVolume);
}

void Sound::VolumeDown()
{
	if (Volume > MIN_VOLUME)
	{
		Volume -= 0.1f;
	}
	FMOD_Channel_SetVolume(m_Channel, Volume * AttVolume);
}

void Sound::SetVolume(float v)
{
	Volume = v;
	FMOD_Channel_SetVolume(m_Channel, Volume * AttVolume);
}

FMOD_BOOL Sound::CheckEndSound()
{
	FMOD_BOOL IsPlaying;
	FMOD_Channel_IsPlaying(m_Channel, &IsPlaying);
	return IsPlaying;
}

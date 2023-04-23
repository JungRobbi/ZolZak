#pragma once
//-----------------------------------------------------------------------------
// File: Timer.h
//-----------------------------------------------------------------------------

// const ULONG MAX_SAMPLE_COUNT = 50; // Maximum frame time sample count

class Timer
{
public:
	static void Tick(float fLockFPS = 0.0f);
	static void Start();
	static void Stop();
	static void Reset();
	static void Initialize();

	static unsigned long GetFrameRate(wchar_t* lpszString = NULL, int nCharacters = 0);
	static float GetTimeElapsed();
	static float GetTotalTime();

private:
	static double							m_fTimeScale;
	static float							m_fTimeElapsed;
	
	static __int64							m_nBasePerformanceCounter;
	static __int64							m_nPausedPerformanceCounter;
	static __int64							m_nStopPerformanceCounter;
	static __int64							m_nCurrentPerformanceCounter;
	static __int64							m_nLastPerformanceCounter;

	static __int64							m_nPerformanceFrequencyPerSec;

	static float							m_fFrameTime[50];
	static unsigned long					m_nSampleCount;

	static unsigned long					m_nCurrentFrameRate;
	static unsigned long					m_nFramesPerSecond;
	static float							m_fFPSTimeElapsed;

	static bool							m_bStopped;
};

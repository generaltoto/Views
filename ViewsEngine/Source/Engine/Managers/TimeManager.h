#pragma once


class TimeManager {
	friend class Engine;
public:

	~TimeManager();


	void Reset();	// Call before message loop.
	void Start();	// Call when unpaused.
	void Stop();	// Call when paused.
	void Tick();	// Call every frame.

	__int64 GetCurrentFrameTime();

	float GetTotalTime();

	static void SetTimeScale(float timeScale);
	static void SetFixedTime(float fixedTime);

	static float GetGameTime()  { return m_CurrTime; }
	static float GetFixedTime() { return m_FixedTime; }

	static float GetUnscaledDeltaTime() { return m_DeltaTime; }
	static float GetDeltaTime() { return m_DeltaTime * m_TimeScale; }

	static float GetTimeScale() { return m_TimeScale; }

private:

	TimeManager();

	__int64 m_performtime;
	float m_freq;

	float m_BaseTime;
	float m_PausedTime;
	float m_StopTime;
	float m_PrevTime;

	static float m_FixedTime;
	static float m_CurrTime;
	static float m_DeltaTime;
	static float m_TimeScale;

	static LARGE_INTEGER m_PerfCountFreq;

	bool isStopped;

};
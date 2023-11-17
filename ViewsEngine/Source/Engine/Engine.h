#pragma once

#include "Core/Core.h"
#include "Engine/Managers/PhysicsWorld.h"
#include "Platform/Windows/Window.h"

class Engine
{
public:

	enum EngineState
	{
		INITIALIZING,
		RUNTIME,
	};

	~Engine();

	static Engine* GetInstance();
	static InputHandler *GetInput();
	static PhysicsWorld *GetPhysicsWorld();
	static CameraComponent *GetMainCamera();
	static Win32::Window* GetWindow();

	static void RestartGame();

	void Initialize(Win32::IApplication* game);
	void Run();
	void Shutdown() const;

	void OnApplicationFocus();
	void OnApplicationLostFocus();

	[[nodiscard]] EngineState GetEngineState() const { return m_EngineState; }
	
	virtual void OnResize();
private:

	Engine();

	void PreInitialize();
	void InitComponents() const;
	
	void TogglePause();

	void Update(float dt);

	static void Render();
	void CalculateFrameStats();
	bool NeedsToClose() const;

	[[nodiscard]] float GetAspectRatio() const { return static_cast<float>(m_Window.GetWidth()) / static_cast<float>(m_Window.GetHeight()); }

private:

	static Engine *m_Instance;
	Coordinator* m_Coordinator;
	TimeManager m_TimeManager;
	CameraManager m_CameraManager;
	InputHandler m_InputHandler;
	PhysicsWorld m_PhysicsWorld;

	Win32::Window m_Window;
	Win32::IApplication* m_Game;

	bool m_IsPaused = false;

	EngineState m_EngineState;
};

#include "Engine.h"

#include "D3D/Base/D3DRenderer.h"

#include <numbers>


Engine* Engine::m_Instance = nullptr;

Engine::Engine(): m_Coordinator(nullptr), m_Game(nullptr), m_EngineState(INITIALIZING)
{
}

Engine::~Engine()
{
    DELPTR(m_Coordinator);
}

Engine* Engine::GetInstance()
{
    if (m_Instance == nullptr)
    {
        m_Instance = new Engine();
    }

    return m_Instance;
}

InputHandler* Engine::GetInput()
{
    return &GetInstance()->m_InputHandler;
}

CameraComponent* Engine::GetMainCamera()
{
    return I(Engine)->m_CameraManager.GetMainCamera();
}

PhysicsWorld* Engine::GetPhysicsWorld()
{
    return &GetInstance()->m_PhysicsWorld;
}

Win32::Window* Engine::GetWindow()
{
    return &GetInstance()->m_Window;
}

#pragma region INIT
void Engine::PreInitialize()
{
    m_Coordinator = Coordinator::GetInstance();
}

void Engine::Initialize(Win32::IApplication* game)
{
    m_Game = game;
    m_EngineState = INITIALIZING;

    PreInitialize();

    m_Coordinator->Init();

    m_Window.CreateNewWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, PerGameSettings::GameName(), PerGameSettings::MainIcon(), Win32::RESIZABLE);
    m_InputHandler.Init(m_Window.GetHandle());
    OnApplicationFocus();

    I(D3DRenderer)->SetState(D3DRenderer::D3DState::INIT);
    I(D3DRenderer)->InitializeD3D12(&m_Window);
}
#pragma endregion

#pragma region MAIN
void Engine::Run()
{
    m_Game->Initialize();

    m_TimeManager.Reset();

    InitComponents();

    OnResize();

    m_EngineState = RUNTIME;
    I(D3DRenderer)->SetState(D3DRenderer::D3DState::RUNTIME);

    while (!NeedsToClose())
    {
        m_Window.PollEvent();

        Sleep(1);
        m_TimeManager.Tick();

        if (m_IsPaused) continue;

        Update(m_TimeManager.GetDeltaTime());

        m_Coordinator->DestroyRegisteredEntities();

        Render();
    }
}

void Engine::InitComponents() const
{
    m_Coordinator->AwakeComponents();
    m_Coordinator->StartComponents();
}

bool Engine::NeedsToClose() const
{
    return m_Window.NeedsToClose();
}

void Engine::RestartGame()
{
    I(Coordinator)->CleanUp();
    GetInstance()->m_PhysicsWorld.CleanUp();
    CameraManager::SetMainCamera(nullptr);

    GetInstance()->Run();
}

void Engine::Update(const float dt)
{
    m_PhysicsWorld.Update(dt);

    m_InputHandler.Update(TimeManager::GetUnscaledDeltaTime());

    m_Coordinator->UpdateSystems(dt);
    m_Coordinator->UpdateComponents();
    m_Coordinator->LateUpdateComponents();

    I(D3DRenderer)->Update(dt, m_TimeManager.GetTotalTime());
    CalculateFrameStats();
}

void Engine::Render()
{
    I(D3DRenderer)->Render();
}
#pragma endregion

void Engine::CalculateFrameStats()
{
    // Code computes the average frames per second, and also the
    // average time it takes to render one frame.  These stats
    // are appended to the window caption bar.

    static int frameCnt = 0;
    static float timeElapsed = 0.0f;

    frameCnt++;

    // Compute averages over one second period.
    if ((m_TimeManager.GetTotalTime() - timeElapsed) >= 1.0f)
    {
        std::wstring windowText;
        auto fps = static_cast<float>(frameCnt); // fps = frameCnt / 1
        float mspf = 1000.0f / fps;

        std::wstring fpsStr = std::to_wstring(fps);
        std::wstring mspfStr = std::to_wstring(mspf);

        windowText = L"    fps: " + fpsStr + L"   mspf: " + mspfStr;

        // Reset for next average.
        frameCnt = 0;
        timeElapsed += 1.0f;
        SetWindowText(m_Window.GetHandle(), windowText.c_str());
    }
}

void Engine::OnResize()
{
    if (const D3DRenderer::D3DState s = I(D3DRenderer)->GetState(); s == D3DRenderer::D3DState::RUNTIME)
        I(D3DRenderer)->OnResize(m_Window.GetWidth(), m_Window.GetHeight());
}

void Engine::Shutdown() const
{
    m_Game->Shutdown();

    DELPTR(m_Instance);

    delete I(D3DRenderer);
}

void Engine::OnApplicationFocus()
{
    if (m_IsPaused)
    {
        TogglePause();
        m_IsPaused = false;
    }

    m_InputHandler.CaptureCursor();
}

void Engine::OnApplicationLostFocus()
{
    if (!m_IsPaused)
    {
        TogglePause();
        m_IsPaused = true;
    }
    m_InputHandler.ReleaseCursor();
}

void Engine::TogglePause()
{
    m_IsPaused = !m_IsPaused;

    if (m_IsPaused)
    {
        m_InputHandler.ReleaseCursor();
    }
    else
    {
        m_InputHandler.CaptureCursor();
    }
}

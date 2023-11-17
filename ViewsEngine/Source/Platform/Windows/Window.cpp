#include "Window.h"

#include "Engine/Engine.h"

namespace Win32
{
    Window::Window()
        : m_Type(RESIZABLE), m_Hwnd(nullptr), m_Title(L"Views"), m_HIcon(nullptr)
    {
    }

    Window::~Window() = default;

    void Window::CreateNewWindow(const int width, const int height, const WSTRING& title, const HICON icon, const WindowType type)
    {
        m_Type = type;
        m_Title = title;
        m_HIcon = icon;

        m_Width = width;
        m_Height = height;

        RegisterNewClass();

        RECT desktop;
        const HWND hDesktop = GetDesktopWindow();
        GetWindowRect(hDesktop, &desktop);

        RECT r = {0, 0, m_Width, m_Height};
        AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, false);

        m_Hwnd = CreateWindow(m_Title.c_str(), m_Title.c_str(), m_Type, ((desktop.right / 2) - (m_Width / 2)),
                              ((desktop.bottom / 2) - (m_Height / 2)), m_Width, m_Height, nullptr, nullptr, HInstance(), (void*)this);

        if (m_Hwnd == nullptr)
        {
            OutputDebugString(L"Window Creation Failed!\n");
            assert(false);
        }

        SetWindowLongPtr(m_Hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        ShowWindow(m_Hwnd, SW_SHOW);
        UpdateWindow(m_Hwnd);
    }

    void Window::PollEvent() const
    {
        MSG msg;
        while (PeekMessage(&msg, m_Hwnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void Window::RegisterNewClass() const
    {
        WNDCLASSEX wcex;

        wcex.cbSize = sizeof(WNDCLASSEX); // define the size of the window class (init)
        wcex.style = CS_HREDRAW | CS_VREDRAW; // Redraw on Horizontal or Vertical movement/resize
        wcex.cbClsExtra = 0; // Extra bytes to allocate following the window-class structure
        wcex.cbWndExtra = 0; // Set to 0 because we doesn't need extra memory for now
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW); // Load the default arrow cursor
        wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(46, 46, 46));
        wcex.hIcon = m_HIcon;
        wcex.hIconSm = m_HIcon;
        wcex.lpszClassName = m_Title.c_str();
        wcex.lpszMenuName = nullptr;
        wcex.hInstance = HInstance(); // Set the instance that we have created
        wcex.lpfnWndProc = WindowProcedure;

        RegisterClassEx(&wcex);
    }

    LRESULT CALLBACK Window::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        auto* window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

        switch (message)
        {
        case WM_QUIT: break;

        case WM_SETFOCUS: I(Engine)->OnApplicationFocus();
            break;

        case WM_KILLFOCUS: I(Engine)->OnApplicationLostFocus();
            break;

        // Event called when the window is resized (every time a resize is done)
        case WM_SIZE:
            // Update the width and height of the window
            SetNewSize(LOWORD(lParam), HIWORD(lParam));
            
            switch (wParam)
            {
            // If the window is maximized, we need to resize our render target
            case SIZE_MAXIMIZED: SetState(MAXIMISED);
                I(Engine)->OnResize();
                break;
            // Nothing to do if the window is minimized
            case SIZE_MINIMIZED: SetState(MINIMISED);
                break;
            // If the window is restored, that means the window is not maximized, but visible (not minimized)
            // We can resize our render target if the user is not resizing the window
            case SIZE_RESTORED: if (m_Resizing == false)
                    I(Engine)->OnResize();

                SetState(RESTORED);
                break;
            default: break;
            }

        // Called when the user start resizing the window
        // We store the fact that the user is resizing the window to prevent the render target to be resized too often
        case WM_ENTERSIZEMOVE: m_Resizing = true;
            break;

        // We finally resize our render target when the user stop resizing the window
        case WM_EXITSIZEMOVE: m_Resizing = false;
            I(Engine)->OnResize();
            break;

        // Called when the user try to minimize the window, we prevent the window to be too small
        case WM_GETMINMAXINFO: reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.x = 200;
            reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.y = 200;
            break;


        case WM_KEYDOWN: if (wParam == VK_ESCAPE)
                I(Engine)->OnApplicationLostFocus();
            break;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN: SetFocus(hwnd);
            I(Engine)->OnApplicationFocus();
            break;

        case WM_CLOSE: window->m_NeedsToClose = true;
            PostQuitMessage(0);
            break;

        case WM_DESTROY: default: break;
        }

        return DefWindowProcW(hwnd, message, wParam, lParam);
    }

    void Window::SetNewSize(const int newWidth, const int newHeight)
    {
        m_Width = newWidth;
        m_Height = newHeight;
    }

    void Window::SetState(const WindowState state)
    {
        m_State = state;
    }
}

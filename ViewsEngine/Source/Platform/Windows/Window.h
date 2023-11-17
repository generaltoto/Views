#pragma once
#include "Win32Utils.h"

namespace Win32
{
    class Window
    {
    public:
        enum WindowState
        {
            MINIMISED,
            MAXIMISED,
            RESTORED
        };
    public:
        Window();
        ~Window();
        
        static void SetNewSize(int newWidth, int newHeight);
        static void SetState(const WindowState state);

        void CreateNewWindow(int width, int height, const WSTRING& title, HICON icon = nullptr, WindowType type = RESIZABLE);
        void PollEvent() const;
        [[nodiscard]] bool NeedsToClose() const { return m_NeedsToClose; }

        [[nodiscard]] int GetWidth() const { return m_Width; }
        [[nodiscard]] int GetHeight() const { return m_Height; }

        [[nodiscard]] HWND GetHandle() const { return m_Hwnd; }
    protected:
        static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    protected:
        inline static int m_Width = DEFAULT_WIDTH;
        inline static int m_Height = DEFAULT_HEIGHT;

        inline static WindowState m_State = WindowState::RESTORED;
        inline static bool m_Resizing = false;

        WindowType m_Type;
        HWND m_Hwnd;
        WSTRING m_Title;
        HICON m_HIcon;
        
    private:
        void RegisterNewClass() const;
    
    private:
        bool m_NeedsToClose = false;
    };
}

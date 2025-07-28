#pragma once
#include "pch.h"
#include "JDWndBase.h"
//namespace JDWindow {
//    class WindowSizeProvider;
//}
namespace JDGlobal {
    namespace Window {
        class WindowSIze {
        public:
            using WindowSizeProvider = JDWindow::WindowSizeProvider;
            static WindowSIze& Instance()
            {
                static WindowSIze instance;

                return instance;
            }
        public:
             void Set(WindowSizeProvider* p);
             WindowSizeProvider* Get();
             int GetWidth() const { assert(m_instance); return m_instance->GetWidth(); }
             int GetHeight() const { assert(m_instance); return m_instance->GetHeight(); }
             D2D1::Matrix3x2F GetViewTM() { assert(m_instance); return D2D1::Matrix3x2F::Translation(m_instance->GetWidth()* 0.5f, m_instance->GetHeight() * 0.5f); }
        private:
            WindowSIze() = default;
            ~WindowSIze() = default;

            WindowSIze(const WindowSIze&) = delete;
            WindowSIze& operator=(const WindowSIze&) = delete;

            WindowSizeProvider* m_instance = nullptr;
        };
    }
}    
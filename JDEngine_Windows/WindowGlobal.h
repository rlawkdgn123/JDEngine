#pragma once
#include "pch.h"
#include "JDWndBase.h"
//namespace JDWindow {
//    class WindowSizeProvider;
//}
namespace JDGlobal {
    namespace Window {
        class WindowSize {
        public:
            using WindowSizeProvider = JDWindow::WindowSizeProvider;
            static WindowSize& Instance()
            {
                static WindowSize instance;

                return instance;
            }
        public:
            void Set(WindowSizeProvider* p);
            WindowSizeProvider* Get();

            int GetWidth() const {
                assert(m_instance);
                return m_instance->GetWidth();
            }

            int GetHeight() const {
                assert(m_instance);
                return m_instance->GetHeight();
            }

            D2D1_SIZE_F GetSize() const {
                assert(m_instance);
                return D2D1_SIZE_F{
                    static_cast<float>(m_instance->GetWidth()),
                    static_cast<float>(m_instance->GetHeight())
                };
            }

            D2D1::Matrix3x2F GetViewTM() const{
                assert(m_instance);
                return D2D1::Matrix3x2F::Translation(
                    m_instance->GetWidth() * 0.5f,
                    m_instance->GetHeight() * 0.5f
                );
            }

            HWND GetHWND() const {
                assert(m_instance);
                return (HWND)m_instance->GetHandle();
            }

        private:
            WindowSize() = default;
            ~WindowSize() = default;

            WindowSize(const WindowSize&) = delete;
            WindowSize& operator=(const WindowSize&) = delete;

            WindowSizeProvider* m_instance = nullptr;
        };
    }
}    
#pragma once
namespace JDWindow {
    class WindowSizeProvider;
}
namespace JDGlobal {
    namespace Window {
        class GlobalWindowSize {
        public:
            using WindowSizeProvider = JDWindow::WindowSizeProvider;
            static GlobalWindowSize& Instance()
            {
                static GlobalWindowSize instance;

                return instance;
            }
        public:
             void Set(WindowSizeProvider* p);
             WindowSizeProvider* Get();

        private:
            GlobalWindowSize() = default;
            ~GlobalWindowSize() = default;

            GlobalWindowSize(const GlobalWindowSize&) = delete;
            GlobalWindowSize& operator=(const GlobalWindowSize&) = delete;

            WindowSizeProvider* m_instance = nullptr;
        };
    }
}    
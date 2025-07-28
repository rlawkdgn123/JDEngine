#include "pch.h"
#include "JDWndBase.h"
#include "WindowGlobal.h"

void JDGlobal::Window::GlobalWindowSize::Set(WindowSizeProvider* provider)
{
	m_instance = provider;
}

JDWindow::WindowSizeProvider* JDGlobal::Window::GlobalWindowSize::Get()
{
	return m_instance;
}

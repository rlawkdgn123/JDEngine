#include "pch.h"
#include "JDWndBase.h"
#include "WindowGlobal.h"

void JDGlobal::Window::WindowSize::Set(WindowSizeProvider* provider)
{
	m_instance = provider;
}

JDWindow::WindowSizeProvider* JDGlobal::Window::WindowSize::Get()
{
	return m_instance;
}

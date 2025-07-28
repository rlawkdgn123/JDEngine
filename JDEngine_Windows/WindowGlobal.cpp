#include "pch.h"
#include "JDWndBase.h"
#include "WindowGlobal.h"

void JDGlobal::Window::WindowSIze::Set(WindowSizeProvider* provider)
{
	m_instance = provider;
}

JDWindow::WindowSizeProvider* JDGlobal::Window::WindowSIze::Get()
{
	return m_instance;
}

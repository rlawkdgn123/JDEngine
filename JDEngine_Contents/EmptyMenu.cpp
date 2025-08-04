#include "pch.h"
#include "framework.h"
#include "EmptyMenu.h"

using namespace std;
using RectTransform = JDComponent::D2DTM::RectTransform;
using PivotPreset = JDComponent::D2DTM::RectTransform::PivotPreset;

using namespace JDGameObject;
using namespace Content;

////////////////////////////////////////////////////////////////////////////////
// // EmptyMenu
////////////////////////////////////////////////////////////////////////////////

void EmptyMenu::Awake()
{
	if (!m_active) { return; }

	if (!m_isAwaked) {
		m_isAwaked = true;
		// cout << "Image Awake" << endl;

		
		
	}
}
void EmptyMenu::Start()
{
	if (!m_active) { return; }

	if (!m_isStarted) {
		m_isStarted = true;
		// cout << "Image Start" << endl;
	}
}
void EmptyMenu::Update(float deltaTime)
{
	if (!m_active) { return; }
	// __super::Update(deltaTime);
}
void EmptyMenu::LateUpdate(float deltaTime)
{
	if (!m_active) { return; }
	// __super::LateUpdate(deltaTime);
}
void EmptyMenu::FixedUpdate(float fixedDeltaTime)
{
	if (!m_active) { return; }
	// __super::FixedUpdate(fixedDeltaTime);
}
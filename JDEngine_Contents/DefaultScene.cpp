#include "pch.h"
#include "framework.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "DefaultScene.h"
using namespace std;
using namespace JDGameObject::Content;
using JDComponent::RenderLayer;
using JDComponent::AnimationRender;
using JDComponent::TextureRenderer;
using JDGlobal::Base::SortingLayer;
using JDGlobal::Math::Vector2F;
using JDComponent::BoxCollider;


namespace JDScene {

    void DefaultScene::OnEnter() {}

    void DefaultScene::OnLeave() {}

    void DefaultScene::Update(float deltaTime) { SceneBase::Update(deltaTime); }

    void DefaultScene::FixedUpdate(float fixedDeltaTime) { SceneBase::FixedUpdate(fixedDeltaTime); }

    void DefaultScene::LateUpdate(float deltaTime) { SceneBase::LateUpdate(deltaTime); }

    void DefaultScene::Render(float dt) {}
}

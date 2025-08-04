#pragma once
namespace JDGameObject {
    namespace Content {
        class Player : public JDGameObject::GameObject
        {
        public:
            Player() : GameObject(L"Player") {}
            Player(const std::wstring& name) : GameObject(name) {}
            void Awake() override;
            void Start() override;                              // ���� 1ȸ�� ȣ��
            void Update(float deltaTime) override;              // Update
            void LateUpdate(float deltaTime) override;          // Update �� ȣ��
            void FixedUpdate(float fixedDeltaTime) override;    // ���� ����
        };
    }
}
#pragma once
namespace JDGameObject {
    namespace Content {
        class House;    // ���� ����
        class Building; // ���� ����
    }
}
namespace JDGameObject {
    namespace Content {
        class Grid : public JDGameObject::GameObject
        {
        public:
            Grid() : GameObject(L"Player") {}
            Grid(const std::wstring& name) : GameObject(name) {}
            void Awake() override;
            void Start() override;                              // ���� 1ȸ�� ȣ��
            void Update(float deltaTime) override;              // Update
            void LateUpdate(float deltaTime) override;          // Update �� ȣ��
            void FixedUpdate(float fixedDeltaTime) override;    // ���� ����

            bool IsExpanded() const { return m_expanded; }
            void SetExpanded(bool expanded) { m_expanded = expanded; }

            bool IsOccupied() const { return m_occupied; }
            void SetOccupied(bool occupied) { m_occupied = occupied; }

            bool HasBuilding() const { return m_hasBuilding; }
            void SetHasBuilding(bool hasBuilding) { m_hasBuilding = hasBuilding; }

            void SetBuilding(const JDGameObject::GameObject* building);

        private:
            bool m_expanded = false; // ���� ���� Ȯ�� ���� ���� ����
            bool m_occupied = false; // ���� Ȯ�� �� ������ �������� ����
            bool m_hasBuilding = false; // �ǹ� ���� ����

            JDGameObject::GameObject* BuildingRaw; // �������� �ǹ� ���� ptr
        };
    }
}
#pragma once
namespace JDGameObject {
    namespace Content {
        class House;    // 전방 선언
        class Building; // 전방 선언
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
            void Start() override;                              // 최초 1회만 호출
            void Update(float deltaTime) override;              // Update
            void LateUpdate(float deltaTime) override;          // Update 후 호출
            void FixedUpdate(float fixedDeltaTime) override;    // 물리 계산용

            bool IsExpanded() const { return m_expanded; }
            void SetExpanded(bool expanded) { m_expanded = expanded; }

            bool IsOccupied() const { return m_occupied; }
            void SetOccupied(bool occupied) { m_occupied = occupied; }

            bool HasBuilding() const { return m_hasBuilding; }
            void SetHasBuilding(bool hasBuilding) { m_hasBuilding = hasBuilding; }

            void SetBuilding(const JDGameObject::GameObject* building);

        private:
            bool m_expanded = false; // 현재 지형 확장 선택 가능 여부
            bool m_occupied = false; // 현재 확장 후 소유한 지형인지 여부
            bool m_hasBuilding = false; // 건물 소유 여부

            JDGameObject::GameObject* BuildingRaw; // 소유중인 건물 참조 ptr
        };
    }
}
#pragma once
#include <mutex>
// Meyer’s Singleton 적용 C++ 11
// 일반 싱글톤과 다르게 첫 호출 시점에 생성됨(지연 초기화)
// function static object를 이용하여 Singleton 구현
// SingletonBase: 템플릿을 상속받는 T만 유일 인스턴스로 생성.
template<typename T>
class SingletonBase
{
public:

    inline static T& Instance()
    {
        static T instance;
        return instance;
    }

protected:
    SingletonBase() = default;
    virtual ~SingletonBase() = default;

private:
    // 복사/이동 금지
    SingletonBase(const SingletonBase&) = delete;
    SingletonBase& operator=(const SingletonBase&) = delete;
    SingletonBase(SingletonBase&&) = delete;
    SingletonBase& operator=(SingletonBase&&) = delete;

    // 오직 T만이 생성·소멸을 제어할 수 있도록 friend 지정
    friend T;
};
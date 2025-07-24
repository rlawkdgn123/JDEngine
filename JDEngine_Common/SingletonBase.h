#pragma once
#include <mutex>
// Meyer��s Singleton ���� C++ 11
// �Ϲ� �̱���� �ٸ��� ù ȣ�� ������ ������(���� �ʱ�ȭ)
// function static object�� �̿��Ͽ� Singleton ����
// SingletonBase: ���ø��� ��ӹ޴� T�� ���� �ν��Ͻ��� ����.
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
    // ����/�̵� ����
    SingletonBase(const SingletonBase&) = delete;
    SingletonBase& operator=(const SingletonBase&) = delete;
    SingletonBase(SingletonBase&&) = delete;
    SingletonBase& operator=(SingletonBase&&) = delete;

    // ���� T���� �������Ҹ��� ������ �� �ֵ��� friend ����
    friend T;
};
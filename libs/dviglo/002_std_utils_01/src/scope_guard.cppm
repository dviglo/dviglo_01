/*
    ��������������� ������, ������� �������� �����-�� �������, ����� �� ������ �� ������� ���������.

    ������ �������������:
    {
        dvScopeGuard sg1([] { cout << "sg1 "; });
        dvScopeGuard sg2 = [] { cout << "sg2 "; };
    }

    ����� �������� "sg2 sg1 " ��� ��� � C++ ����������� ����������� � ��������� ������� ������� �������������.
*/

export module dviglo.scope_guard;

import <concepts>;

using namespace std;

export template<typename F>
// ����������, ��� ��������� ����� ����������
requires invocable<F> && requires(F f) { { f() } -> same_as<void>; }
class dvScopeGuard
{
private:
    F func_;

public:
    // ��������� ���������� ������
    dvScopeGuard(const dvScopeGuard&) = delete;
    dvScopeGuard& operator=(const dvScopeGuard&) = delete;

    dvScopeGuard(F&& func)
        : func_(func)
    {
    }

    // � ����������� �������� �������
    ~dvScopeGuard()
    {
        func_();
    }
};

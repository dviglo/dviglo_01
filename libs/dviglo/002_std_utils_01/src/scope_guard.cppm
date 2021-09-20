/*
    Вспомогательный объект, который выполнит какую-то функцию, когда он выйдет из области видимости.

    Пример использования:
    {
        DvScopeGuard sg1([] { cout << "sg1 "; });
        DvScopeGuard sg2 = [] { cout << "sg2 "; };
    }

    Будет выведено "sg2 sg1 " так как в C++ деструкторы выполняются в реверсном порядке вызовов конструкторов.
*/

export module dviglo.scope_guard;

import <concepts>;

using namespace std;

export template<typename F>
// Убеждаемся, что сигнатура лямбы правильная
requires invocable<F> && requires(F f) { { f() } -> same_as<void>; }
class DvScopeGuard
{
private:
    F func_;

public:
    // Запрещаем копировать объект
    DvScopeGuard(const DvScopeGuard&) = delete;
    DvScopeGuard& operator=(const DvScopeGuard&) = delete;

    DvScopeGuard(F&& func)
        : func_(func)
    {
    }

    // В деструкторе вызываем функцию
    ~DvScopeGuard()
    {
        func_();
    }
};

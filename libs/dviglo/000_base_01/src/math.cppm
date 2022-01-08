export module dviglo.base.math;

// Стандартная библиотека
import <limits>;

using namespace std;

export
{

// Сравнивает два числа с плавающей запятой
// https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
template<typename T>
inline typename enable_if<!numeric_limits<T>::is_integer, bool>::type
dv_almost_equal(T x, T y, int ulp = 2)
{
    return fabs(x - y) <= numeric_limits<T>::epsilon() * fabs(x + y) * ulp
        || fabs(x - y) < numeric_limits<T>::min();
}

} // export

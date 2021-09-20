export module dviglo.std_vector_utils;

// Стандартная библиотека
import <algorithm>; // find

using namespace std;

export
{

template <typename T1, typename T2>
inline bool contains(T1 vec, T2 val)
{
    return find(vec.begin(), vec.end(), val) != vec.end();
}

} // export

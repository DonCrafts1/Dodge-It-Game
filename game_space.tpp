#include "game_space.h"
// #include "util.h"

template <typename T, typename... X>
inline T* GameSpace::instantiate(X... args)
{
    static_assert(std::is_base_of<GameObject, T>::value, "");
    T* obj = new T(args...);
    GameSpace::get_instance()->entities.push_front(obj);
    return obj;
}

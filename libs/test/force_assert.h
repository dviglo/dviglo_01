#pragma once

// Этот дефайн определён в релизной версии и отключает assert(). Нам же нужно, чтобы assert() работал даже в релизной версии
#undef NDEBUG

#include <cassert>

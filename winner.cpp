#include "winner.h"

#ifndef NDEBUG
void winner::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }
}
#endif

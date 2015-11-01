#include "clickcard.h"

#ifndef NDEBUG
void click_card::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }
}
#endif

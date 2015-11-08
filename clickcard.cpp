#include "clickcard.h"

#ifndef NDEBUG
void ribi::imcw::click_card::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }
}
#endif

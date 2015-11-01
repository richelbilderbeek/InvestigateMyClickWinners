#include "winner.h"

#include <cassert>

winner::winner()
  : m_value_euros{0.0}
{
  #ifndef NDEBUG
  test();
  #endif
}

void winner::add_value_euros(const double euros)
{
  assert(euros >= 0.0 && "Winners can only increase in value");
  m_value_euros += euros;
}

#ifndef NDEBUG
void winner::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }
  {
    winner w;
    assert(w.get_value_euros() == 0.0);
  }
}
#endif

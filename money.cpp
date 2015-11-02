#include "money.h"

#include "helper.h"

#include <cassert>
#include <cmath>

money::money(const double value_in_euros)
  : m_euros{value_in_euros}
{
  #ifndef NDEBUG
  test();
  #endif
}

#ifndef NDEBUG
void money::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }

  //is_about_equal
  {
    assert(money(0.0) == money(0.0000));
    assert(money(0.0) == money(0.0049));
    assert(money(0.0) != money(0.0051));
  }
}
#endif


bool operator==(const money& a, const money& b) noexcept
{
  return std::abs(a.m_euros - b.m_euros) < 0.005;
}

bool operator!=(const money& a, const money& b) noexcept
{
  return !(a == b);
}

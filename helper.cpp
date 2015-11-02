#include "helper.h"

#include <cassert>
#include <cmath>

helper::helper()
{
  #ifndef NDEBUG
  test();
  #endif
}

bool helper::is_about_equal(const double a, const double b) const noexcept
{
  return std::abs(a - b) < 0.005;
}

#ifndef NDEBUG
void helper::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }
  const helper h;

  //is_about_equal
  {
    assert( h.is_about_equal(0.0,0.0000));
    assert( h.is_about_equal(0.0,0.0049));
    assert(!h.is_about_equal(0.0,0.0051));
  }
}
#endif


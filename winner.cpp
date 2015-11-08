#include "winner.h"

#include <cassert>
#include <iostream>

ribi::imcw::winner::winner(const std::string& owner_name)
  : m_balance{balance("Winner of " + owner_name,0.0)}
{
  #ifndef NDEBUG
  test();
  #endif
}

#ifndef NDEBUG
void ribi::imcw::winner::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }
  {
    winner w("Test winner");
    assert(w.get_value() == 0.0);
  }
}
#endif

std::ostream& ribi::imcw::operator<<(std::ostream& os, const winner& w) noexcept
{
  os << w.get_value();
  return os;
}

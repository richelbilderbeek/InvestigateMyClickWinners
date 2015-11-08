#include "winner.h"

#include <cassert>
#include <iostream>

ribi::imcw::winner::winner()
  : m_value_euros{0.0}
{
  #ifndef NDEBUG
  test();
  #endif
}

/*
void ribi::imcw::winner::add_value_euros(const double euros)
{
  assert(euros >= 0.0 && "Winners can only increase in value");
  m_value_euros += euros;
}
*/

#ifndef NDEBUG
void ribi::imcw::winner::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }
  {
    winner w;
    assert(w.get_value() == balance(0.0));
  }
}
#endif

std::ostream& ribi::imcw::operator<<(std::ostream& os, const winner& w) noexcept
{
  os << w.get_value();
  return os;
}

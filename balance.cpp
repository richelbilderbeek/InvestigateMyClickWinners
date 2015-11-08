#include "balance.h"

#include "helper.h"

#include <cassert>
#include <cmath>
#include <iostream>

ribi::imcw::balance::balance(
  const std::string& description,
  const double value_in_euros)
  : m_description{description},
    m_value{value_in_euros}
{
  #ifndef NDEBUG
  test();
  #endif
}

#ifndef NDEBUG
void ribi::imcw::balance::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }
}
#endif

std::ostream& ribi::imcw::operator<<(std::ostream& os, const balance& b) noexcept
{
  os
    << b.get_description() << ", value: "
    << b.get_value() << " euros"
  ;
  return os;
}

bool ribi::imcw::operator==(const balance& a, const balance& b) noexcept
{
  return
       a.get_description() == b.get_description()
    && a.get_value() == b.get_value()
  ;
}

bool ribi::imcw::operator!=(const balance& a, const balance& b) noexcept
{
  return !(a == b);
}

#include "balance.h"

#include "helper.h"

#include <cassert>
#include <cmath>
#include <iostream>

ribi::imcw::balance::balance(
  const std::string& description,
  const double value_in_euros)
  : m_description{description},
    m_euros{value_in_euros}
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

  //is_about_equal
  {
    assert(balance("a",0.0) == balance("a",0.0000));
    assert(balance("a",0.0) == balance("a",0.0049));
    assert(balance("a",0.0) != balance("a",0.0051));
  }

}
#endif


bool ribi::imcw::operator==(const balance& a, const balance& b) noexcept
{
  return std::abs(a.m_euros - b.m_euros) < 0.005;
}

bool ribi::imcw::operator!=(const balance& a, const balance& b) noexcept
{
  return !(a == b);
}

bool ribi::imcw::operator<(const balance& a, const balance& b) noexcept
{
  return a.get_value_euros() < b.get_value_euros();
}

bool ribi::imcw::operator>(const balance& a, const balance& b) noexcept
{
  return a.get_value_euros() > b.get_value_euros();
}

bool ribi::imcw::operator>=(const balance& a, const balance& b) noexcept
{
  return !(a < b);
}

std::ostream& ribi::imcw::operator<<(std::ostream& os, const balance& b) noexcept
{
  os
    << b.get_description() << ", value: "
    << b.get_value_euros() << " euros"
  ;
  return os;
}

/*
ribi::imcw::balance ribi::imcw::operator*(const balance& b, const double multiplier)
{
  balance c(
   b.get_value_euros() * multiplier
  );
  return c;
}
*/

#include "money.h"

#include <cassert>
#include <cmath>
#include <iostream>

ribi::imcw::money::money(
  const double value_in_euros
)
  : m_value_euros{value_in_euros}
{
  #ifndef NDEBUG
  test();
  #endif
}

#ifndef NDEBUG
void ribi::imcw::money::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }
  {
    const money a(3.14);
    const money b(a);
    assert(a == b);
  }
  {
    const money a(3.14);
    const money b(2.71);
    assert(a != b);
  }
  {
    const money a(3.14);
    money b(2.71);
    assert(a != b);
    b = a;
    assert(a == b);
  }
  {
    assert(money(0.0) == money(0.0000));
    assert(money(0.0) == money(0.0049));
    assert(money(0.0) != money(0.0051));
  }
  {
    assert(money(1.0) + money(2.0) == money(3.0));
    assert(money(2.0) + money(1.0) == money(3.0));
  }
  {
    assert(money(2.0) - money(1.0) == money(1.0));
    assert(money(3.0) - money(1.0) == money(2.0));
  }
}
#endif

std::ostream& ribi::imcw::operator<<(std::ostream& os, const money& m) noexcept
{
  os
    << m.get_value_euros() << " euros"
  ;
  return os;
}

bool ribi::imcw::operator==(const money& a, const money& b) noexcept
{
  return std::abs(a.get_value_euros() - b.get_value_euros()) < 0.005;
}

bool ribi::imcw::operator!=(const money& a, const money& b) noexcept
{
  return !(a == b);
}

bool ribi::imcw::operator<(const money& a, const money& b) noexcept
{
  return a.get_value_euros() < b.get_value_euros();
}

bool ribi::imcw::operator>(const money& a, const money& b) noexcept
{
  return a.get_value_euros() > b.get_value_euros();
}

bool ribi::imcw::operator>=(const money& a, const money& b) noexcept
{
  return !(a < b);
}

ribi::imcw::money ribi::imcw::operator+(const money& lhs, const money& rhs) noexcept
{
  return money(lhs.get_value_euros() + rhs.get_value_euros());
}

ribi::imcw::money ribi::imcw::operator-(const money& lhs, const money& rhs) noexcept
{
  return money(lhs.get_value_euros() - rhs.get_value_euros());
}

ribi::imcw::money ribi::imcw::operator-(const money& lhs) noexcept
{
  return money(-lhs.get_value_euros());
}

ribi::imcw::money ribi::imcw::operator*(const money& m, const double multiplier)
{
  return money(m.get_value_euros() * multiplier);
}

ribi::imcw::money ribi::imcw::operator*(const double multiplier, const money& m)
{
  return m * multiplier;
}

ribi::imcw::money ribi::imcw::operator/(const money& m, const double denominator)
{
  assert(denominator > 0.0);
  money n(m.get_value_euros() / denominator);
  return n;
}

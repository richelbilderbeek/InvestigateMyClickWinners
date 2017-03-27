#include "money.h"

#include <cassert>
#include <cmath>
#include <iostream>

ribi::imcw::money::money(
  const double value_in_euros
) : m_value_euros{value_in_euros}
{

}

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

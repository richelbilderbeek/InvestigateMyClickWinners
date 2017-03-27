#include "balance.h"

#include "helper.h"

#include <cassert>
#include <cmath>
#include <iostream>

ribi::imcw::balance::balance(
  const std::string& description,
  const money& initial_value
) : m_description{description},
    m_value{initial_value}
{

}

std::ostream& ribi::imcw::operator<<(std::ostream& os, const balance& b) noexcept
{
  os
    << b.get_description() << ", value: "
    << b.get_value()
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

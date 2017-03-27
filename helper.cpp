#include "helper.h"

#include <cassert>
#include <cmath>

ribi::imcw::helper::helper()
{

}

bool ribi::imcw::helper::is_about_equal(const double a, const double b) const noexcept
{
  return std::abs(a - b) < 0.005;
}

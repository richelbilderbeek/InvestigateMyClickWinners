#include "winner_package.h"

#include <cassert>

winner_package::winner_package(const winner_package_name name)
  : m_n_winners{calculate_n_winners(name)}
{
  #ifndef NDEBUG
  test();
  #endif
}

constexpr int calculate_n_winners(const winner_package_name name)
{
  switch (name)
  {
    case winner_package_name::starter  : return  1;
    case winner_package_name::basic    : return  3;
    case winner_package_name::junior   : return  5;
    case winner_package_name::senior   : return 10;
    case winner_package_name::master   : return 25;
    case winner_package_name::executive: return 50;
  }
  return 0;
}

#ifndef NDEBUG
void winner_package::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }
  assert(calculate_n_winners(winner_package_name::starter  ) ==  1);
  assert(calculate_n_winners(winner_package_name::basic    ) ==  3);
  assert(calculate_n_winners(winner_package_name::junior   ) ==  5);
  assert(calculate_n_winners(winner_package_name::senior   ) == 10);
  assert(calculate_n_winners(winner_package_name::master   ) == 25);
  assert(calculate_n_winners(winner_package_name::executive) == 50);
}
#endif

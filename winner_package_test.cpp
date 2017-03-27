#include "winner_package.h"


void ribi_imcw_winner_package_test() noexcept
{
  assert(calculate_n_winners(winner_package_name::starter  ) ==  1);
  assert(calculate_n_winners(winner_package_name::basic    ) ==  3);
  assert(calculate_n_winners(winner_package_name::junior   ) ==  5);
  assert(calculate_n_winners(winner_package_name::senior   ) == 10);
  assert(calculate_n_winners(winner_package_name::master   ) == 25);
  assert(calculate_n_winners(winner_package_name::executive) == 50);
}

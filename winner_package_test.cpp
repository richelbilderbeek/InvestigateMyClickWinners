#include "winner_package.h"


#include <boost/test/unit_test.hpp>

using namespace ribi::imcw;

BOOST_AUTO_TEST_CASE(imcw_winner_package)
{
  BOOST_CHECK(calculate_n_winners(winner_package_name::starter  ) ==  1);
  BOOST_CHECK(calculate_n_winners(winner_package_name::basic    ) ==  3);
  BOOST_CHECK(calculate_n_winners(winner_package_name::junior   ) ==  5);
  BOOST_CHECK(calculate_n_winners(winner_package_name::senior   ) == 10);
  BOOST_CHECK(calculate_n_winners(winner_package_name::master   ) == 25);
  BOOST_CHECK(calculate_n_winners(winner_package_name::executive) == 50);
}

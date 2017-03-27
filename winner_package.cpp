#include "winner_package.h"

#include <cassert>

ribi::imcw::winner_package::winner_package(const winner_package_name name)
  : m_n_winners{calculate_n_winners(name)}
{

}

int ribi::imcw::calculate_n_winners(const winner_package_name name)
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

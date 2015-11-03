#include "simulation.h"

#include <algorithm>
#include <boost/date_time/gregorian/gregorian.hpp>

simulation::simulation(const simulation_parameters& parameters)
  : m_company{},
    m_parameters{parameters}
{

}

void simulation::run() noexcept
{
  company c;

  //focal_person buy his/her membership
  person focal_person = m_parameters.get_focal_person();
  c.buy_winner_package(focal_person,winner_package_name::starter,focal_person.get_balance_euros());

  //add others
  std::vector<person> others = m_parameters.get_others();
  std::for_each(
    std::begin(others),
    std::end(others),
    [&c](auto& p) { c.add(p); }
  );

  //Display initial situation
  std::cout << c << std::endl;

  //Do the sim
  for (
    auto today = m_parameters.get_start();
    today != m_parameters.get_end();
    today += boost::gregorian::days(1)
  ) {
    std::cout << "today: " << today << std::endl;

  }

}

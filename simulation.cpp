#include "simulation.h"

#include <algorithm>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "calendar.h"

simulation::simulation(const simulation_parameters& parameters)
  : m_company{},
    m_parameters{parameters}
{

}

void simulation::run() noexcept
{
  company the_company;

  //focal_person buy his/her membership
  person focal_person = m_parameters.get_focal_person();
  the_company.buy_winner_package(focal_person,winner_package_name::starter,focal_person.get_balance_euros());

  //add others
  std::vector<person> others = m_parameters.get_others();
  std::for_each(
    std::begin(others),
    std::end(others),
    [&the_company](auto& p) { the_company.add(p); }
  );

  //Display initial situation
  std::cout << "*****************" << std::endl;
  std::cout << "Initial situation" << '\n';
  std::cout << "*****************" << std::endl;
  std::cout << the_company << std::endl;
  std::cout << "*****************" << std::endl;

  calendar the_calendar(m_parameters.get_start());

  //Do the sim
  for (
  ;
    the_calendar.get_current_day() != m_parameters.get_end();
    the_calendar.go_to_next_day()
  ) {
    std::cout << "today: " << the_calendar.get_current_day() << std::endl;

    if (the_calendar.distibute_profit_winners_today()) {
      const double winners_net_profit = 100.0;
      std::cout << "***************************" << std::endl;
      std::cout << "Distributing Winners profit of " << winners_net_profit << " euros" << std::endl;
      std::cout << "***************************" << std::endl;
      the_company.distribute_net_profit(winners_net_profit);
      std::cout << the_company << std::endl;
      std::cout << "***************************" << std::endl;
    }
    if (the_calendar.distibute_profit_webshop_today()) {
      const double webshop_net_profit = 1.0;
      std::cout << "***************************" << std::endl;
      std::cout << "Distributing webshop profit of " << webshop_net_profit << " euros" << std::endl;
      std::cout << "***************************" << std::endl;
      the_company.distribute_net_profit(webshop_net_profit);
      std::cout << the_company << std::endl;
      std::cout << "***************************" << std::endl;
    }

  }

  //Display final situation
  std::cout << "***************" << std::endl;
  std::cout << "Final situation" << '\n';
  std::cout << "***************" << std::endl;
  std::cout << the_company << std::endl;
}

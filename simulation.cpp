#include "simulation.h"

#include <algorithm>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "bank.h"
#include "calendar.h"

ribi::imcw::simulation::simulation(
  const bank& any_bank,
  const calendar& any_calendar,
  const company& any_company,
  const simulation_parameters& parameters
)
  : m_bank{any_bank},
    m_calendar{any_calendar},
    m_company{any_company},
    m_parameters{parameters}
{

}

void ribi::imcw::simulation::run() noexcept
{

  //focal_person buy his/her membership
  person focal_person = m_parameters.get_focal_person();
  m_company.buy_winner_package(
    focal_person,
    winner_package_name::starter,
    focal_person.get_balance_euros(),
    m_bank,
    m_calendar
  );

  //add others
  std::vector<person> others = m_parameters.get_others();
  std::for_each(
    std::begin(others),
    std::end(others),
    [this](auto& p) { m_company.add(p); }
  );

  //Display initial situation
  std::cout << "*****************" << std::endl;
  std::cout << "Initial situation" << '\n';
  std::cout << "*****************" << std::endl;
  std::cout << m_company << std::endl;
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
      balance winners_net_profit(
        "MCW Winners profit",
        100.0
      );
      std::cout << "***************************" << std::endl;
      std::cout << "Distributing Winners profit of " << winners_net_profit << " euros" << std::endl;
      std::cout << "***************************" << std::endl;
      m_company.distribute_net_profit(
        winners_net_profit,
        m_bank,
        m_calendar
      );
      assert(winners_net_profit.get_value_euros() == 0.0);
      std::cout << m_company << std::endl;
      std::cout << "***************************" << std::endl;
    }
    if (the_calendar.distibute_profit_webshop_today()) {
      balance webshop_net_profit(
        "MCW webshop profit",
        1.0
      );
      std::cout << "***************************" << std::endl;
      std::cout << "Distributing webshop profit of " << webshop_net_profit << " euros" << std::endl;
      std::cout << "***************************" << std::endl;
      m_company.distribute_net_profit(
        webshop_net_profit,
        m_bank,
        m_calendar
      );
      assert(webshop_net_profit.get_value_euros() == 0.0);
      std::cout << m_company << std::endl;
      std::cout << "***************************" << std::endl;
    }

  }

  //Display final situation
  std::cout << "***************" << std::endl;
  std::cout << "Final situation" << '\n';
  std::cout << "***************" << std::endl;

  std::cout << "***************" << std::endl;
  std::cout << "Company" << '\n';
  std::cout << "***************" << std::endl;
  std::cout << m_company << std::endl;
  std::cout << "***************" << std::endl;
  std::cout << "Bank" << '\n';
  std::cout << "***************" << std::endl;
  std::cout << m_bank << std::endl;
}

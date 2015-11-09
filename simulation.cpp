#include "simulation.h"

#include <algorithm>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "bank.h"
#include "calendar.h"

ribi::imcw::simulation::simulation(
  const simulation_parameters& parameters
)
  : m_bank{},
    m_calendar(parameters.get_start()),
    m_company{},
    m_parameters{parameters}
{
  //focal_person buy his/her membership
  person focal_person = m_parameters.get_focal_person();
  m_company.buy_winner_package(
    focal_person,
    winner_package_name::starter,
    focal_person.get_balance(),
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

}

void ribi::imcw::simulation::run() noexcept
{
  //Do the sim
  for (
  ;
    m_calendar.get_today() != m_parameters.get_end();
    m_calendar.go_to_next_day()
  ) {
    std::cout << "today: " << m_calendar.get_today() << std::endl;

    //Website
    if (m_calendar.transfer_profit_website_today()) {
      balance website_net_profit(
        "Monthly website profit",100.0
      );
      m_company.transfer(website_net_profit,m_bank,m_calendar);
    }
    //Webshop
    if (m_calendar.transfer_profit_webshop_today()) {
      balance webshop_net_profit(
        "Yearly webshop profit",100.0
      );
      m_company.transfer(webshop_net_profit,m_bank,m_calendar);
    }
    //Winners
    //No transfer, all Winner money is already on the undistributed balance

    //Distribute profit
    if (m_calendar.distribute_profit_today()) {
      std::cout << "***************************" << std::endl;
      std::cout << "Distributing profit" << std::endl;
      std::cout << "***************************" << std::endl;
      m_company.distribute_net_profit(
        m_bank,
        m_calendar
      );
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

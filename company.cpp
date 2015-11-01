#include "company.h"

#include <cmath>
#include <cassert>
#include <sstream>
#include <iostream>

#include "winner_package.h"
company::company()
  : m_balance_compensation_plan_euros{0.0},
    m_balance_holding_euros{0.0},
    m_balance_reserves_euros{0.0},
    m_balance_undistributed_euros{0.0},
    m_winners{}
{
  #ifndef NDEBUG
  test();
  #endif
}

void company::buy(person& customer, const winner_package_name name)
{
  if (customer.has_click_card())
  {
    std::stringstream s;
    s << __func__
      << "Cannot buy a WinnerPackage when customer already has a ClickCard";
    throw std::logic_error(s.str());
  }
  const winner_package p(name);
  const int n_winners = p.get_n_winners();

  //ClickCard
  {
    click_card c;
    customer.pay(c);
    m_balance_undistributed_euros += c.cost_inc_vat_euros;
  }
  //Winners
  for (int i=0; i!=n_winners; ++i)
  {
    auto w = std::make_shared<winner>();
    m_winners.push_back(w);
    customer.pay(w);
    m_balance_undistributed_euros += winner::prive_vat_exempt_euros;
  }

}

void company::distribute_net_profit(const double money_euros) noexcept
{
  const double change_compensation_plan_euros = money_euros * proportion_of_profit_to_compensation_plan;
  const double change_holding_euros           = money_euros * proportion_of_profit_to_holding;
  const double change_reserves_euros          = money_euros * proportion_of_profit_to_reserves;
  const double change_winners_euros           = money_euros * proportion_of_profit_to_winners;

  m_balance_compensation_plan_euros += change_compensation_plan_euros;
  m_balance_holding_euros += change_holding_euros;
  m_balance_reserves_euros += change_reserves_euros;

  const int n_winners = get_n_winners();
  const double income_per_winners_euros
    = change_winners_euros
    / static_cast<double>(n_winners)
  ;
  for (const auto p: m_winners) { p->add_value_euros(income_per_winners_euros); }
}

#ifndef NDEBUG
void company::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }
  //When a company is started, all balances are zero
  {
    company c;
    assert(c.get_balance_compensation_plan_euros() == 0.0);
    assert(c.get_balance_holding_euros () == 0.0);
    assert(c.get_balance_reserves_euros() == 0.0);
    //assert(c.get_customers().empty());
  }
  //When a company is started, all balances are zero
  {
    company c;
    person p;
    c.buy(p,winner_package_name::basic);
    assert(c.get_balance_compensation_plan_euros() == 0.0);
    assert(c.get_balance_holding_euros () == 0.0);
    assert(c.get_balance_reserves_euros() == 0.0);
    //assert(c.get_customers().empty());
  }
  //A person buying a starter winner package results in 100 euros in the undistributed money balance
  {
    person p;
    company c;
    c.buy(p,winner_package_name::starter);
    const double expected = 100.0;
    const double observed = c.get_balance_undistributed_euros();
    assert(std::abs(expected - observed) < 0.005);
  }
  //A person buying an executive winner package has to pay 60+(50*40) euros in the undistributed money balance
  {
    person p;
    company c;
    c.buy(p,winner_package_name::executive);
    const double expected = 2060.0;
    const double observed = c.get_balance_undistributed_euros();
    assert(std::abs(expected - observed) < 0.005);
  }
  //When a company distributes profit, it is distributed as expected
  {
    company c;
    person p;
    c.buy(p,winner_package_name::starter);
    assert(c.get_balance_compensation_plan_euros() == 0.0);
    assert(c.get_balance_holding_euros () == 0.0);
    assert(c.get_balance_reserves_euros() == 0.0);
    //100 euros is distributed
    c.distribute_net_profit(100.0);
    assert(c.get_balance_compensation_plan_euros() == 15.0);
    assert(c.get_balance_holding_euros () == 10.0);
    assert(c.get_balance_reserves_euros() == 30.0);
    assert(p.get_winners().size() == 1);
    assert(p.get_winners()[0]);
    assert(p.get_winners()[0]->get_value_euros() == 45.0);
  }
}
#endif

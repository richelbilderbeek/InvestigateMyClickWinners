#include "company.h"

#include <algorithm>
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
    m_customers{}
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
  assert(!is_customer(customer));


  #ifndef NDEBUG
  const auto n_customers_before = m_customers.size();
  #endif // NDEBUG

  const winner_package p(name);
  const int n_winners = p.get_n_winners();

  //ClickCard, makes person a customer
  {
    click_card c;
    customer.pay(c);
    m_customers.push_back(customer);
    m_balance_undistributed_euros += c.cost_inc_vat_euros;
  }
  //Winners
  for (int i=0; i!=n_winners; ++i)
  {
    winner w;
    customer.pay(w);
    m_balance_undistributed_euros += winner::prive_vat_exempt_euros;
  }
  assert(customer.get_n_winners() == n_winners);

  #ifndef NDEBUG
  const auto n_customers_after = m_customers.size();
  assert(n_customers_after == n_customers_before + 1);
  #endif // NDEBUG

}

std::vector<std::reference_wrapper<winner>> company::collect_winners() noexcept
{
  std::vector<std::reference_wrapper<winner>> v;
  for (const auto& p: m_customers)
  {
    auto winners = p.get().get_winners();
    std::copy(
      std::begin(winners),
      std::end(winners),
      std::back_inserter(v)
    );
  }
  return v;
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

  //Collect the Winners from all customers
  const auto winners = collect_winners();

  //Distribute the money over the winners
  const int n_winners{static_cast<int>(winners.size())};
  const double income_per_winners_euros
    = change_winners_euros
    / static_cast<double>(n_winners)
  ;
  for (auto& winner: winners)
  {
    winner.get().add_value_euros(income_per_winners_euros);
  }
}

bool company::is_customer(const person& p) const noexcept
{
  const auto iter = std::find_if(
    std::begin(m_customers),
    std::end(m_customers),
    [p](const auto customer) { return p == customer; }
  );
  return iter != std::end(m_customers);
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
    assert(p.get_winners()[0].get_value_euros() == 45.0);
  }
  //Winners end when exceeding 50 euros
  {
    company c;
    person p;
    c.buy(p,winner_package_name::starter);
    assert(c.get_balance_compensation_plan_euros() == 0.0);
    assert(c.get_balance_holding_euros () == 0.0);
    assert(c.get_balance_reserves_euros() == 0.0);
    //100 euros is distributed
    //customer will have one Winner with 45 euro on it
    c.distribute_net_profit(100.0);
    assert(c.get_balance_compensation_plan_euros() == 15.0);
    assert(c.get_balance_holding_euros () == 10.0);
    assert(c.get_balance_reserves_euros() == 30.0);
    assert(p.get_winners().size() == 1);
    assert(p.get_winners()[0].get_value_euros() == 45.0);
    assert(p.get_bank_wallet_euros() == 0.0);
    //100 euros is distributed again
    //customer had one Winner with 45 euro on it, so this goes to 50,
    //and will get a new Winner with 40 euro on it
    //the 10 euros from the first winner is distributed
    // - 75% to BankWallet, equals 7.50 euros
    // - 25% to ShopWallet, equals 2.50 euros
    c.distribute_net_profit(100.0);
    assert(c.get_balance_compensation_plan_euros() == 30.0);
    assert(c.get_balance_holding_euros () == 20.0);
    assert(c.get_balance_reserves_euros() == 60.0);
    assert(p.get_winners().size() == 1);
    assert(p.get_winners()[0].get_value_euros() == 40.0);
    assert(p.get_bank_wallet_euros() == 7.50);
    assert(p.get_shop_wallet_euros() == 2.50);
  }
  assert(!"Green");
}
#endif

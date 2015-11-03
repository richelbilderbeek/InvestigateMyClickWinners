#include "company.h"

#include <algorithm>
#include <cmath>
#include <cassert>
#include <sstream>
#include <iostream>

#include "helper.h"
#include "winner.h"
#include "winner_package.h"

company::company()
  : m_balance_compensation_plan_euros{0.0},
    m_balance_holding_euros{0.0},
    m_balance_reserves_euros{0.0},
    m_balance_undistributed_euros{0.0},
    m_customers{},
    m_verbose{false}
{
  #ifndef NDEBUG
  test();
  #endif
}

void company::add(person& customer)
{
  if (
    std::find(
      std::begin(m_customers),
      std::end(m_customers),
      customer
    ) != std::end(m_customers)
  ) {
    std::stringstream msg;
    msg
      << __func__ << ": cannot add existing customer (ID: "
      << customer.get_id() << ") again"
    ;
    throw std::logic_error(msg.str());
  }

  m_customers.push_back(customer);
}

#ifndef NDEBUG
void company::ban(const person& customer)
{
  if(!is_customer(customer))
  {
    std::stringstream s;
    s << __func__
      << "Cannot ban a person that is not a customer";
    throw std::logic_error(s.str());
  }

  const auto iter = std::find(
    std::begin(m_customers),
    std::end(m_customers),
    customer
  );

  (*iter).get().remove_card();
  (*iter).get().get_winners().resize(0);
  std::swap(*iter,m_customers.back());
  assert(!m_customers.empty());
  m_customers.pop_back();

  assert(!is_customer(customer));
}
#endif

void company::buy_winner(person& customer, double& account_euros)
{
  winner w;
  account_euros                 -= winner::price_vat_exempt_euros;
  m_balance_undistributed_euros += winner::price_vat_exempt_euros;
  customer.add_winner(w);
}

void company::buy_winner_package(
  person& customer,
  const winner_package_name name,
  double& account_euros
)
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

  //Buy ClickCard
  {
    click_card c;
    account_euros                 -= c.cost_inc_vat_euros;
    m_balance_undistributed_euros += c.cost_inc_vat_euros;
    customer.add_click_card(c);
  }
  assert(customer.has_click_card());

  //ClickCard makes person a customer
  m_customers.push_back(customer);

  //Winners
  for (int i=0; i!=n_winners; ++i)
  {
    buy_winner(customer,account_euros);
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
  for (auto& p: m_customers)
  {
    std::vector<winner>& winners = p.get().get_winners();
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
  auto winners = collect_winners();

  //Distribute the money over the winners
  const int n_winners{static_cast<int>(winners.size())};
  assert(n_winners > 0);

  const double income_per_winners_euros
    = change_winners_euros
    / static_cast<double>(n_winners)
  ;
  for (auto& winner: winners)
  {
    winner.get().add_value_euros(income_per_winners_euros);
  }

  if (m_verbose) { std::clog << "Process the Winners" << std::endl; }
  for (auto& customer: m_customers)
  {
    customer.get().process_winners(*this);
  }

  if (m_verbose)
  {
    std::clog
      << "Distributing " << change_winners_euros << " euros over the winners\n"
      << "n_winners: " << n_winners << '\n'
      << "income_per_winners_euros: " << income_per_winners_euros << " euros\n"
    ;
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
  const helper h;
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
    c.buy_winner_package(p,winner_package_name::basic,p.get_balance_euros());
    assert(c.get_balance_compensation_plan_euros() == 0.0);
    assert(c.get_balance_holding_euros () == 0.0);
    assert(c.get_balance_reserves_euros() == 0.0);
    //assert(c.get_customers().empty());
  }
  //When a person buys a WinnerPackage he/she is added to the company
  {
    company c;
    assert(c.get_customers().empty());
    person p;
    c.buy_winner_package(p,winner_package_name::starter,p.get_balance_euros());
    assert(c.get_customers().size() == 1);
  }
  //A person buying a starter winner package results in 100 euros in the undistributed money balance
  {
    person p;
    company c;
    c.buy_winner_package(p,winner_package_name::starter,p.get_balance_euros());
    const double expected = 100.0;
    const double observed = c.get_balance_undistributed_euros();
    assert(h.is_about_equal(expected,observed));
  }
  //A person buying an executive winner package has to pay 60+(50*40) euros in the undistributed money balance
  {
    person p;
    company c;
    c.buy_winner_package(p,winner_package_name::executive,p.get_balance_euros());
    const double expected = 2060.0;
    const double observed = c.get_balance_undistributed_euros();
    assert(h.is_about_equal(expected,observed));
  }
  //When a person is banned, his/her ClickCard is removed
  {
    company c;
    person p;
    c.buy_winner_package(p,winner_package_name::starter,p.get_balance_euros());
    assert(p.has_click_card());
    c.ban(p);
    assert(!p.has_click_card());
  }
  //When a person is banned, his/her Winners are removed
  {
    company c;
    person p;
    c.buy_winner_package(p,winner_package_name::starter,p.get_balance_euros());
    assert(!p.get_winners().empty());
    c.ban(p);
    assert( p.get_winners().empty());
  }
  //When a company distributes profit, it is distributed as expected
  {
    company c;
    person p;
    c.buy_winner_package(p,winner_package_name::starter,p.get_balance_euros());
    assert(c.get_balance_compensation_plan_euros() == 0.0);
    assert(c.get_balance_holding_euros () == 0.0);
    assert(c.get_balance_reserves_euros() == 0.0);
    //100 euros is distributed, this ends up one the 1 winner of the 1 customer
    c.distribute_net_profit(100.0);
    assert(h.is_about_equal(c.get_balance_compensation_plan_euros(),15.0));
    assert(h.is_about_equal(c.get_balance_holding_euros (),10.0));
    assert(h.is_about_equal(c.get_balance_reserves_euros(),30.0));
    assert(p.get_winners().size() == 1);
    assert(c.get_customers().size() == 1);
    assert(c.get_customers()[0].get().get_winners().size() == 1);
    assert(
      h.is_about_equal(
        c.get_customers()[0].get().get_winners()[0].get_value_euros(),
        45.0
      )
    );
    assert(h.is_about_equal(p.get_winners()[0].get_value_euros(),45.0));
  }
  //Winners end when exceeding 50 euros and are converted to
  //BankWallet and ShopWallet when customer does not automatically
  //buy new Winners
  {
    company c;
    person p;
    p.set_auto_buy(false);
    c.buy_winner_package(p,winner_package_name::starter,p.get_balance_euros());
    assert(c.get_balance_compensation_plan_euros() == 0.0);
    assert(c.get_balance_holding_euros () == 0.0);
    assert(c.get_balance_reserves_euros() == 0.0);
    //200 euros is distributed
    //customer will have one Winner with 90 euro on it,
    //that will break down
    c.distribute_net_profit(2.0 * 100.0);
    assert(c.get_balance_compensation_plan_euros() == 2.0 * 15.0);
    assert(c.get_balance_holding_euros () == 2.0 * 10.0);
    assert(c.get_balance_reserves_euros() == 2.0 * 30.0);
    assert(p.get_winners().size() == 0);
    assert(p.get_bank_wallet_euros() == 87.50);
    assert(p.get_shop_wallet_euros() ==  2.50);
  }
  //Winners end when exceeding 50 euros and are converted to
  //a new Winner, BankWallet and ShopWallet,
  //as customer does automatically buy new Winners
  {
    company c;
    person p;
    c.buy_winner_package(p,winner_package_name::starter,p.get_balance_euros());
    assert(c.get_balance_compensation_plan_euros() == 0.0);
    assert(c.get_balance_holding_euros () == 0.0);
    assert(c.get_balance_reserves_euros() == 0.0);
    //200 euros is distributed
    //customer will have one Winner with 90 euro on it,
    //that will break down
    c.distribute_net_profit(2.0 * 100.0);
    assert(c.get_balance_compensation_plan_euros() == 2.0 * 15.0);
    assert(c.get_balance_holding_euros () == 2.0 * 10.0);
    assert(c.get_balance_reserves_euros() == 2.0 * 30.0);
    assert(p.get_winners().size() == 2);
    assert(p.get_bank_wallet_euros() == 7.50);
    assert(p.get_shop_wallet_euros() == 2.50);
  }
}
#endif

std::ostream& operator<<(std::ostream& os, const company& c) noexcept
{
  os
    << "Balance compensation plan: " << c.m_balance_compensation_plan_euros << " euros" << '\n'
    << "Balance holding: " << c.m_balance_holding_euros << " euros" << '\n'
    << "Balance reserves: " << c.m_balance_reserves_euros << " euros" << '\n'
    << "Balance undistributed: " << c.m_balance_undistributed_euros << " euros" << '\n'
    << "#customers: " << c.m_customers.size() << '\n'
  ;
  for (const auto& d: c.m_customers) { std::cout << d << std::endl; }
  return os;
}

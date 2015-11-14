#include "company.h"

#include <algorithm>
#include <cmath>
#include <cassert>
#include <sstream>
#include <iostream>

#include "buy_winners_strategy.h"
#include "bank.h"
#include "calendar.h"
#include "helper.h"
#include "person.h"
#include "money.h"
#include "winner.h"
#include "winner_package.h"

ribi::imcw::company::company()
  : m_balance_compensation_plan{"CompensationPlan"},
    m_balance_holding{"Holding"},
    m_balance_reserves{"Reserves"},
    m_balance_undistributed{"Undistributed"},
    m_customers{},
    m_verbose{false}
{
  #ifndef NDEBUG
  test();
  #endif
}

void ribi::imcw::company::add(person& customer)
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
void ribi::imcw::company::ban(const person& customer)
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
  (*iter).get().get_winners().clear();
  std::swap(*iter,m_customers.back());
  assert(!m_customers.empty());
  m_customers.pop_back();

  assert(!is_customer(customer));
}
#endif

void ribi::imcw::company::buy_click_card(
  person& customer,
  balance& account_euros,
  bank& the_bank,
  calendar& the_calendar
)
{
  assert(customer.has_account(account_euros));
  #ifndef NDEBUG
  const auto before = account_euros.get_value();
  #endif
  the_bank.transfer(
    account_euros,
    money(click_card::cost_inc_vat_euros),
    m_balance_undistributed,
    the_calendar.get_today()
  );

  #ifndef NDEBUG
  const auto after = account_euros.get_value();
  assert(after < before);
  #endif

  click_card c(the_calendar.get_today());
  customer.add_click_card(c);
}

void ribi::imcw::company::buy_winner(
  person& customer,
  balance& account_euros,
  bank& the_bank,
  calendar& the_calendar
)
{
  #ifndef NDEBUG
  assert(customer.has_account(account_euros));
  const auto before = account_euros.get_value();
  #endif

  the_bank.transfer(
    account_euros,
    money(winner::price_vat_exempt_euros),
    m_balance_undistributed,
    the_calendar.get_today()
  );

  #ifndef NDEBUG
  const auto after = account_euros.get_value();
  assert(after < before);
  #endif

  winner w(customer.get_name());
  customer.add_winner(w);
}


void ribi::imcw::company::buy_winner_package(
  person& customer,
  const winner_package_name name,
  balance& account_euros,
  bank& the_bank,
  calendar& the_calendar
)
{
  if (customer.has_valid_click_card(the_calendar.get_today()))
  {
    std::stringstream s;
    s << __func__
      << "Cannot buy a WinnerPackage when customer already has a ClickCard";
    throw std::logic_error(s.str());
  }
  assert(!is_customer(customer));
  assert(customer.has_account(account_euros));

  #ifndef NDEBUG
  const auto account_before = account_euros.get_value();
  const auto n_customers_before = m_customers.size();
  #endif // NDEBUG

  const winner_package p(name);
  const int n_winners = p.get_n_winners();

  //Buy ClickCard
  {
    the_bank.transfer(
      account_euros,
      money(click_card::cost_inc_vat_euros),
      m_balance_undistributed,
      the_calendar.get_today()
    );
    click_card c(the_calendar.get_today());
    assert(c.is_valid(the_calendar.get_today()));
    customer.add_click_card(c);
  }
  assert(customer.has_valid_click_card(the_calendar.get_today()));

  //ClickCard makes person a customer
  m_customers.push_back(customer);

  //Winners
  for (int i=0; i!=n_winners; ++i)
  {
    buy_winner(customer,account_euros,the_bank,the_calendar);
  }
  assert(customer.get_n_winners() == n_winners);

  #ifndef NDEBUG
  const auto account_after = account_euros.get_value();
  assert(account_after < account_before);
  const auto n_customers_after = m_customers.size();
  assert(n_customers_after == n_customers_before + 1);
  #endif // NDEBUG

}

std::vector<std::reference_wrapper<ribi::imcw::winner>> ribi::imcw::company::collect_winners() noexcept
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

void ribi::imcw::company::distribute_net_profit(
  bank& the_bank,
  calendar& the_calendar
  ) noexcept
{
  balance& source = m_balance_undistributed;
  const auto change_compensation_plan_euros = source.get_value() * proportion_of_profit_to_compensation_plan;
  const auto change_holding_euros = source.get_value() * proportion_of_profit_to_holding;
  const auto change_reserves_euros = source.get_value() * proportion_of_profit_to_reserves;
  const auto change_winners_euros = source.get_value() * proportion_of_profit_to_winners;

  the_bank.transfer(
    source,
    change_compensation_plan_euros,
    m_balance_compensation_plan,
    the_calendar.get_today()
  );

  the_bank.transfer(
    source,
    change_holding_euros,
    m_balance_holding,
    the_calendar.get_today()
  );

  the_bank.transfer(
    source,
    change_reserves_euros,
    m_balance_reserves,
    the_calendar.get_today()
  );

  //Collect the Winners from all customers
  std::vector<std::reference_wrapper<winner>> winners = collect_winners();

  //Distribute the money over the winners
  //const int n_winners{count_winners()};
  const int n_winners{static_cast<int>(winners.size())};
  if (n_winners == 0) {
    //Transfer money to reserves
    const money winners_money = source.get_value();
    the_bank.transfer(
      source,
      winners_money,
      m_balance_reserves,
      the_calendar.get_today()
    );
    return;
  }

  assert(n_winners > 0);

  const money income_per_winners_euros
    = change_winners_euros
    / static_cast<double>(n_winners)
  ;
  for (std::reference_wrapper<winner>& w: winners)
  {
    #ifndef NDEBUG
    const auto winner_value_before = w.get().get_value();
    #endif

    the_bank.transfer(
      source,
      income_per_winners_euros,
      w.get().get_balance(),
      the_calendar.get_today()
    );

    #ifndef NDEBUG
    const auto winner_value_after = w.get().get_value();
    assert(winner_value_after >= winner_value_before);
    #endif
  }

  if (m_verbose) { std::clog << "Process the Winners" << std::endl; }
  for (person& customer: m_customers)
  {
    customer.process_winners(
      the_bank,
      the_calendar,
      *this
    );
  }

  if (m_verbose)
  {
    std::clog
      << "Distributing " << change_winners_euros << " over the winners\n"
      << "n_winners: " << n_winners << '\n'
      << "income_per_winners_euros: " << income_per_winners_euros << " \n"
    ;
  }
}

bool ribi::imcw::company::is_customer(const person& p) const noexcept
{
  const auto iter = std::find_if(
    std::begin(m_customers),
    std::end(m_customers),
    [p](const Customer& customer) { return p == customer; }
  );
  return iter != std::end(m_customers);
}

#ifndef NDEBUG
void ribi::imcw::company::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }
  const helper h;
  //When a company is started, all balances are zero
  {
    bank b;
    calendar c;
    person p("Mr A");
    company mcw;
    assert(mcw.get_balance_compensation_plan().get_value() == money(0.0));
    assert(mcw.get_balance_holding ().get_value() == money(0.0));
    assert(mcw.get_balance_reserves().get_value() == money(0.0));
    //assert(c.get_customers().empty());
  }
  //When a company is started, all balances are zero
  {
    bank b;
    calendar c;
    person p("Mr B");
    company mcw;
    mcw.buy_winner_package(p,winner_package_name::basic,p.get_balance(),b,c);
    assert(mcw.get_balance_compensation_plan().get_value() == money(0.0));
    assert(mcw.get_balance_holding ().get_value() == money(0.0));
    assert(mcw.get_balance_reserves().get_value() == money(0.0));
    //assert(c.get_customers().empty());
  }
  //When a person buys a WinnerPackage he/she is added to the company
  {
    bank b;
    calendar c;
    person p("Mr C");
    company mcw;
    assert(mcw.get_customers().empty());
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,c);
    assert(mcw.get_customers().size() == 1);
  }
  //A person buying a starter winner package results in 100 euros in the undistributed money balance
  {
    bank b;
    calendar c;
    person p("Mr D");
    company mcw;
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,c);
    const money expected(100.0);
    const auto observed = mcw.get_balance_undistributed().get_value();
    assert(expected == observed);
  }
  //A person buying an executive winner package has to pay 60+(50*40) euros in the undistributed money balance
  {
    bank b;
    calendar c;
    person p("Mr E");
    company mcw;
    mcw.buy_winner_package(p,winner_package_name::executive,p.get_balance(),b,c);
    const money expected{2060.0};
    const auto observed = mcw.get_balance_undistributed().get_value();
    assert(expected == observed);
  }
  //When a person is banned, his/her ClickCard is removed
  {
    bank b;
    calendar c;
    person p("Mr F");
    company mcw;
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,c);
    assert(p.has_valid_click_card(c.get_today()));
    mcw.ban(p);
    assert(!p.has_valid_click_card(c.get_today()));
  }
  //When a person is banned, his/her Winners are removed
  {
    bank b;
    calendar c;
    person p("Mr G");
    company mcw;
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,c);
    assert(!p.get_winners().empty());
    mcw.ban(p);
    assert(p.get_winners().empty());
  }
  //When a company distributes profit, it is distributed as expected
  {
    bank b;
    calendar c;
    person p("Mr H");
    company mcw;
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,c);
    assert(mcw.get_balance_compensation_plan().get_value() == money(0.0));
    assert(mcw.get_balance_holding ().get_value() == money(0.0));
    assert(mcw.get_balance_reserves().get_value() == money(0.0));
    assert(mcw.get_balance_undistributed().get_value() == money(100.0));
    //the 100 euros that was undistibuted
    //gets distributed, this ends up one the 1 winner of the 1 customer
    mcw.distribute_net_profit(b,c);
    //std::cerr << mcw.get_balance_compensation_plan().get_value() << std::endl;
    assert(mcw.get_balance_compensation_plan().get_value() == money(15.0));
    assert(mcw.get_balance_holding ().get_value() == money(10.0));
    assert(mcw.get_balance_reserves().get_value() == money(30.0));
    assert(p.get_winners().size() == 1);
    assert(mcw.get_customers().size() == 1);
    assert(mcw.get_customers()[0].get().get_winners().size() == 1);
    assert(
        mcw.get_customers()[0].get().get_winners()[0].get_value()
         == money(45.0)
    );
    assert(p.get_winners()[0].get_value() == money(45.0));
  }
  //Winners end when exceeding 50 euros and are converted to
  //BankWallet and ShopWallet when customer does not automatically
  //buy new Winners
  {
    bank b;
    calendar c;
    person p("Mr I");
    company mcw;
    p.set_winner_buy_strategy(never_buy());
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,c);
    assert(mcw.get_balance_compensation_plan().get_value() == money(0.0));
    assert(mcw.get_balance_holding ().get_value() == money(0.0));
    assert(mcw.get_balance_reserves().get_value() == money(0.0));
    assert(mcw.get_balance_undistributed().get_value() == money(100.0));
    //200 euros is distributed (100 already from winners, 100 from test net profit)
    //customer will have one Winner with 90 euro on it,
    //that will break down
    balance net_profit("test net profit",money(100.0));
    mcw.transfer(net_profit,b,c);
    mcw.distribute_net_profit(b,c);
    assert(net_profit.get_value() == money(0.0));
    assert(mcw.get_balance_compensation_plan().get_value() == money(2.0 * 15.0));
    assert(mcw.get_balance_holding ().get_value() == money(2.0 * 10.0));
    assert(mcw.get_balance_reserves().get_value() == money(2.0 * 30.0));
    assert(p.get_winners().size() == 0);
    assert(p.get_bank_wallet().get_value() == money(87.50));
    assert(p.get_shop_wallet().get_value() ==  money(2.50));
  }
  //Winners end when exceeding 50 euros and are converted to
  //a new Winner, BankWallet and ShopWallet,
  //as customer does automatically buy new Winners
  {
    bank b;
    calendar c;
    person p("Mr J");
    company mcw;
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,c);
    assert(mcw.get_balance_compensation_plan().get_value() == money(0.0));
    assert(mcw.get_balance_holding ().get_value() == money(0.0));
    assert(mcw.get_balance_reserves().get_value() == money(0.0));
    //200 euros is distributed (100 already from winners, 100 from test net profit)
    //He/she has one empty winner
    // - 90 euros go to customer
    //   - winners: 1x 90.0 euros
    //   - ShopWallet: 0.0 euros
    //   - BankWallet: 0.0 euros
    // - full winner will be destroyed:
    //   - winners: 0x 0.0 euros
    //   - ShopWallet: 2.50 euros
    //   - BankWallet: 87.50 euros
    // - automatically, two winners will be bought:
    //   - winners: 2x 0.0 euros
    //   - ShopWallet: 2.50 euros
    //   - BankWallet: 7.50 euros
    //
    //customer will have one Winner with 40 euro on it,
    //that will break down
    balance net_profit("test net profit",money(100.0));
    mcw.transfer(net_profit,b,c);
    mcw.distribute_net_profit(b,c);
    assert(net_profit.get_value() == money(0.0));
    assert(mcw.get_balance_compensation_plan().get_value() == money(2.0 * 15.0));
    assert(mcw.get_balance_holding ().get_value() == money(2.0 * 10.0));
    assert(mcw.get_balance_reserves().get_value() == money(2.0 * 30.0));
    assert(p.get_winners().size() == 2);
    assert(p.get_winners()[0].get_value() == money(0.0));
    assert(p.get_winners()[1].get_value() == money(0.0));
    assert(p.get_bank_wallet().get_value() == money(7.50));
    assert(p.get_shop_wallet().get_value() == money(2.50));
  }
}
#endif

void ribi::imcw::company::transfer(
  balance& source,
  bank& the_bank,
  calendar& the_calendar
) noexcept
{
  const money the_money = source.get_value();
  the_bank.transfer(
    source,
    the_money,
    m_balance_undistributed,
    the_calendar.get_today()
  );
}

std::ostream& ribi::imcw::operator<<(std::ostream& os, const company& c) noexcept
{
  std::stringstream s;
  s
    << "Balance compensation plan: " << c.m_balance_compensation_plan << '\n'
    << "Balance holding: " << c.m_balance_holding << '\n'
    << "Balance reserves: " << c.m_balance_reserves << '\n'
    << "Balance undistributed: " << c.m_balance_undistributed << '\n'
    << "#customers: " << c.m_customers.size() << '\n'
  ;
  for (const person& d: c.m_customers) {
    s << d << '\n';
  }
  std::string t{s.str()};
  assert(!t.empty());
  t.pop_back();
  os << t;
  return os;
}

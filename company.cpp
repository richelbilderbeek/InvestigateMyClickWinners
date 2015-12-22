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
  : m_balance_compensation_plan{"MCW CompensationPlan"},
    m_balance_holding{"MCW holding"},
    m_balance_reserves{"MCW reserves"},
    m_balance_undistributed{"MCW undistributed profit"},
    m_balance_winners{"MCW Winners"},
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
  assert(!m_customers.empty());
  std::swap(*iter,m_customers.back());
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

  //ClickCard will be valid the first day of the next month
  click_card c(
    the_calendar.get_today() //Purchase date
  );
  customer.add_click_card(c);
}

void ribi::imcw::company::buy_winner(
  person& customer,
  balance& account_euros,
  bank& the_bank,
  const boost::gregorian::date& the_day
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
    the_day
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
  const boost::gregorian::date& the_day
)
{
  if (customer.has_valid_click_card(the_day))
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
      the_day
    );
    click_card c(the_day);
    assert(!c.is_valid(the_day)); //Not valid yet
    customer.add_click_card(c);
  }
  assert(!customer.has_valid_click_card(the_day)); //Not valid yet

  //ClickCard makes person a customer
  m_customers.push_back(customer);

  //Winners
  for (int i=0; i!=n_winners; ++i)
  {
    buy_winner(customer,account_euros,the_bank,the_day);
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
    //Don't use Winners directly after first puchase
    if (!p.has_active_winners()) continue;
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
  const money profit = m_balance_undistributed.get_value();
  const money to_compensation_plan = profit * proportion_of_profit_to_compensation_plan;
  const money to_holding = profit * proportion_of_profit_to_holding;
  const money to_reserves = profit * proportion_of_profit_to_reserves;
  const money to_winners = profit * proportion_of_profit_to_winners;

  the_bank.transfer(
    m_balance_undistributed, //Sender
    to_compensation_plan,
    m_balance_compensation_plan, //Target
    the_calendar.get_today()
  );

  the_bank.transfer(
    m_balance_undistributed, //Sender
    to_holding,
    m_balance_holding, //Target
    the_calendar.get_today()
  );

  the_bank.transfer(
    m_balance_undistributed, //Sender
    to_reserves,
    m_balance_reserves, //Target
    the_calendar.get_today()
  );

  //Shortly transfer it to the Winners balance...
  the_bank.transfer(
    m_balance_undistributed, //Sender
    to_winners,
    m_balance_winners, //Target
    the_calendar.get_today()
  );
  //then distribute all over the customers
  distribute_net_profit_winners(
    m_balance_winners, //Sender
    to_winners,
    the_bank,
    the_calendar
  );


  distribute_net_profit_compensation_plan(
    m_balance_compensation_plan, //Sender
    to_compensation_plan,
    the_bank,
    the_calendar
  );

}

void ribi::imcw::company::distribute_net_profit_compensation_plan(
  balance& source,
  const money& total_money,
  bank& the_bank,
  calendar& the_calendar
) noexcept
{
  if (count_active_customers() == 0) {
    //Transfer money to reserves
    the_bank.transfer(
      source,
      total_money,
      m_balance_undistributed,
      the_calendar.get_today()
    );
    return;
  }

  assert(count_active_customers() > 0);
  assert(!"Not implemented yet");
  /*
  const money income_per_winners_euros
    = total_money
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
      << "Distributing " << total_money << " over the winners\n"
      << "n_winners: " << n_winners << '\n'
      << "income_per_winners_euros: " << income_per_winners_euros << " \n"
    ;
  }
  */
}


void ribi::imcw::company::distribute_net_profit_winners(
  balance& source,
  const money& total_money,
  bank& the_bank,
  calendar& the_calendar
) noexcept
{
  //Collect the Winners from all customers
  std::vector<std::reference_wrapper<winner>> winners = collect_winners();

  //Distribute the money over the winners
  //const int n_winners{count_winners()};
  const int n_winners{static_cast<int>(winners.size())};
  if (n_winners == 0) {
    //Transfer money to reserves
    the_bank.transfer(
      source,
      total_money,
      m_balance_reserves,
      the_calendar.get_today()
    );
    return;
  }

  assert(n_winners > 0);

  const money income_per_winners_euros
    = total_money
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
      << "Distributing " << total_money << " over the winners\n"
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
  using boost::gregorian::date;
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
    mcw.buy_winner_package(p,winner_package_name::basic,p.get_balance(),b,date(2015,6,15));
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
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,date(2015,6,15));
    assert(mcw.get_customers().size() == 1);
  }
  //A person buying a starter winner package results in 100 euros in the undistributed money balance
  {
    bank b;
    calendar c;
    person p("Mr D");
    company mcw;
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,date(2015,6,15));
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
    mcw.buy_winner_package(p,winner_package_name::executive,p.get_balance(),b,date(2015,6,15));
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
    mcw.buy_winner_package(
      p,
      winner_package_name::starter,
      p.get_balance(),
      b,
      date(2015,6,15)
    );
    assert(!p.has_valid_click_card(date(2015,6,15)));
    assert( p.has_valid_click_card(date(2015,7,1)));
    mcw.ban(p);
    assert(!p.has_valid_click_card(date(2015,7,1)));
  }
  //When a person is banned, his/her Winners are removed
  {
    bank b;
    calendar c;
    person p("Mr G");
    company mcw;
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,date(2015,6,15));
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
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,date(2015,6,15));
    assert(mcw.get_balance_compensation_plan().get_value() == money(0.0));
    assert(mcw.get_balance_holding ().get_value() == money(0.0));
    assert(mcw.get_balance_reserves().get_value() == money(0.0));
    assert(mcw.get_balance_undistributed().get_value() == money(100.0));
    //the 100 euros that was undistibuted
    //gets distributed, this ends up one the 1 winner of the 1 customer
    mcw.distribute_net_profit(b,c);
    // * compensation_plan: 15%
    // * holding: 10%
    // * reserves: 30%
    // * winners: 45%
    // Thus in this context:
    // * compensation_plan: 15% of 100 euros = 15 euros.
    //     Because no active customers, these 15 euros get transferred to undistributed
    // * holding: 10% of 100 euros = 10 euros
    // * reserves: 30% of 100 euros = 30 euros
    // * winners: 45% of 100 euros = 45 euros.
    //     Because there is one customer with one Winners, this Winner will become 45 euros
    //  * undistributed: 15 euros from compensation plan
    assert(mcw.get_balance_compensation_plan().get_value() == money(0.0));
    assert(mcw.get_balance_holding ().get_value() == money(10.0));
    assert(mcw.get_balance_reserves().get_value() == money(30.0));
    assert(mcw.get_balance_undistributed().get_value() == money(15.0));
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
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,date(2015,6,15));
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
    // * compensation_plan: 15%
    // * holding: 10%
    // * reserves: 30%
    // * winners: 45%
    // Thus in this context:
    // * compensation_plan: 15% of 200 euros = 30 euros.
    //     Because no active customers, these 30 euros get transferred to undistributed
    // * holding: 10% of 200 euros = 20 euros
    // * reserves: 30% of 200 euros = 60 euros
    // * winners: 45% of 200 euros = 90 euros.
    //     Then customer will have one Winner with 90 euro on it, that will expire,
    //     resulting in 2.50 euros in the ShopWallet and 87.50 in the BankWallet
    //  * undistributed: 30 euros from compensation plan
    assert(net_profit.get_value() == money(0.0));
    assert(mcw.get_balance_compensation_plan().get_value() == money(0.0));
    assert(mcw.get_balance_holding ().get_value() == money(20.0));
    assert(mcw.get_balance_reserves().get_value() == money(60.0));
    assert(mcw.get_balance_undistributed().get_value() == money(30.0));
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
    date start_date(2015,6,15);
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,start_date);
    assert(mcw.get_balance_compensation_plan().get_value() == money(0.0));
    assert(mcw.get_balance_holding ().get_value() == money(0.0));
    assert(mcw.get_balance_reserves().get_value() == money(0.0));
    //200 euros is distributed (100 already from winners, 100 from test net profit)
    //He/she has one empty winner
    balance net_profit("test net profit",money(100.0));
    mcw.transfer(net_profit,b,c);
    mcw.distribute_net_profit(b,c);
    // * compensation_plan: 15%
    // * holding: 10%
    // * reserves: 30%
    // * winners: 45%
    // Thus in this context:
    // * compensation_plan: 15% of 200 euros = 30 euros.
    //     Because no active customers, these 30 euros get transferred to undistributed
    // * holding: 10% of 200 euros = 20 euros
    // * reserves: 30% of 200 euros = 60 euros
    // * winners: 45% of 200 euros = 90 euros.
    //     Then customer will have one Winner with 90 euro on it, that will expire,
    //     resulting in 2.50 euros in the ShopWallet and 87.50 in the BankWallet.
    //     Because auto_buy is on, the customer will buy two Winners at 40 euros each,
    //     resulting in:
    //     * ShopWallet of 2.50 euros
    //     * BankWallet of 7.50 euros
    //     * 2x Winner of 40 euros
    //  * undistributed:
    //     * 30 euros from compensation plan
    //     * 2x Winner of 40 euros
    assert(net_profit.get_value() == money(0.0));
    assert(mcw.get_balance_compensation_plan().get_value() == money(0.0));
    assert(mcw.get_balance_holding ().get_value() == money(20.0));
    assert(mcw.get_balance_reserves().get_value() == money(60.0));
    assert(mcw.get_balance_undistributed().get_value() == money(110.0));
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

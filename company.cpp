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
    std::vector<winner>& winners = p.get().get_winners();
    std::copy(
      std::begin(winners),
      std::end(winners),
      std::back_inserter(v)
    );
  }
  return v;
}

std::vector<std::reference_wrapper<ribi::imcw::winner>> ribi::imcw::company::collect_active_winners(
  const boost::gregorian::date& d
) noexcept
{
  std::vector<std::reference_wrapper<winner>> v;
  for (auto& p: m_customers)
  {
    if (p.get().has_active_winners(d)) {
      std::vector<winner>& winners = p.get().get_winners();
      std::copy(
        std::begin(winners),
        std::end(winners),
        std::back_inserter(v)
      );
    }
  }
  return v;
}

int ribi::imcw::company::count_active_customers(
  const boost::gregorian::date& d
) const noexcept
{
  return std::count_if(
    std::begin(m_customers), std::end(m_customers),
    [d](const Customer& p) {
      return p.get().has_active_winners(d);
    }
  );
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
  if (count_active_customers(the_calendar.get_today()) == 0) {
    //Transfer money to reserves
    the_bank.transfer(
      source,
      total_money,
      m_balance_undistributed,
      the_calendar.get_today()
    );
    return;
  }

  assert(count_active_customers(the_calendar.get_today()) > 0);
  #ifdef FIX_ISSUE_5
  assert(!"Not implemented yet");
  #else
  //STUB: Transfer money to reserves
  the_bank.transfer(
    source,
    total_money,
    m_balance_undistributed,
    the_calendar.get_today()
  );
  return;
  #endif
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

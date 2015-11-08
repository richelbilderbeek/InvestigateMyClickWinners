#include "person.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>

#include "bank.h"
#include "calendar.h"
#include "company.h"
#include "helper.h"
#include "winner_package.h"

int ribi::imcw::person::sm_current_id = 0;

ribi::imcw::person::person(
  const std::string& name
) noexcept
  :
    m_auto_buy{true},
    m_balance_euros{"Personal balance of " + name,0.0},
    m_bank_wallet_euros{"BankWallet of " + name,0.0},
    m_card{},
    m_id{sm_current_id++},
    m_name{name},
    m_shop_wallet_euros{"ShopWallet of " + name,0.0},
    m_winners{}
{
  #ifndef NDEBUG
  test();
  #endif
}

void ribi::imcw::person::add_click_card(const click_card& w)
{
  assert(!has_click_card());
  m_card.push_back(w);
  assert(has_click_card());
}


void ribi::imcw::person::add_winner(const winner& w)
{
  m_winners.push_back(w);
}

/*
void ribi::imcw::person::give_income(const double money_euros) noexcept
{
  const auto prev_money = m_bank_wallet_euros + m_shop_wallet_euros;
  m_bank_wallet_euros += (proportion_of_profit_to_bank_wallet * money_euros);
  m_shop_wallet_euros += (proportion_of_profit_to_shop_wallet * money_euros);

  const auto new_money = m_bank_wallet_euros + m_shop_wallet_euros;
  assert(prev_money + money_euros == new_money);
}
*/

bool ribi::imcw::person::has_account(const balance& an_account) const noexcept
{
  return
       an_account == m_balance_euros
    || an_account == m_bank_wallet_euros
    || an_account == m_shop_wallet_euros
  ;
}

void ribi::imcw::person::process_winners(
  bank& the_bank,
  calendar& the_calendar,
  company& the_company
)
{
  //Partition is Winners that will remain
  //and Winners that will be converted to BankWallet and ShopWallet
  const auto iter = std::partition(
    std::begin(m_winners),
    std::end(m_winners),
    [](const auto& w)
    {
      return w.get_value() < winner::max_value_euros;
    }
  );
  //Convert the Winners that will be converted to BankWallet and ShopWallet
  std::for_each(
    iter,
    std::end(m_winners),
    [this,&the_bank,the_calendar](auto& w)
    {
      //const double to_shop_wallet{euros_from_full_winner_to_shopwallet};
      assert(w.get_value() >= winner::max_value_euros);
      assert(w.get_value() >= winner::max_value_euros);
      //Transfer euros_from_full_winner_to_shopwallet from Winner to ShopWallet
      the_bank.transfer(
        w.get_balance(),
        euros_from_full_winner_to_shopwallet,
        m_shop_wallet_euros,
        the_calendar.get_current_day()
      );
      assert(w.get_value() > 0.0);
      assert(w.get_value() > 40.0);
      //Transfer rest from Winner to BankWallet
      the_bank.transfer(
        w.get_balance(),
        w.get_value(),
        m_bank_wallet_euros,
        the_calendar.get_current_day()
      );
    }
  );
  //Remove the Winners that have been converted
  m_winners.erase(iter,std::end(m_winners));

  assert(
    std::count_if(
      std::begin(m_winners),
      std::end(m_winners),
      [](const auto& w) { return w.is_full(); }
    ) == 0
  );

  if (m_auto_buy)
  {
    while (m_bank_wallet_euros.get_value_euros() >= winner::price_vat_exempt_euros)
    {
      const auto bank_wallet_before = m_bank_wallet_euros;
      the_company.buy_winner(*this,m_bank_wallet_euros,the_bank,the_calendar);
      const auto bank_wallet_after = m_bank_wallet_euros;
      assert(bank_wallet_after < bank_wallet_before);
    }
  }
}

void ribi::imcw::person::remove_card()
{
  if (!has_click_card())
  {
    std::stringstream s;
    s << __func__
      << "Cannot remove a ClickCard when customer has none";
    throw std::logic_error(s.str());
  }
  assert(m_card.size() == 1);
  m_card.pop_back();
}

#ifndef NDEBUG
void ribi::imcw::person::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }
  const helper h;
  const boost::gregorian::date today = boost::gregorian::day_clock::local_day();
  //A person that has not spend anything has a neutral balance
  {
    bank b;
    calendar c;
    person p("Mrs A");
    const auto expected_euros{0.0};
    const auto observed = p.get_balance_euros().get_value_euros();
    assert(expected_euros == observed);
  }
  //A person buying a starter winner package has to pay 100 euros
  {
    bank b;
    calendar c;
    person p("Mrs B");
    company mcw;
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance_euros(),b,c);
    const auto expected{-100.0};
    const auto observed = p.get_balance_euros().get_value_euros();
    assert(expected == observed);
  }
  //A person buying an executive winner package has to pay 60+(50*40) euros
  {
    bank b;
    calendar c;
    person p("Mrs C");
    company mwc;
    mwc.buy_winner_package(p,winner_package_name::executive,p.get_balance_euros(),b,c);
    const auto expected{-2060.0};
    const auto observed = p.get_balance_euros().get_value_euros();
    assert(expected == observed);
  }
  //A person his/her profit is distributed over BankWallet and ShopWallet
  {
    bank b;
    calendar c;
    person p("Mrs D");
    balance sender("test");
    const double profit_euros(40.0);
    b.transfer(
      sender,
      profit_euros,
      ribi::imcw::person::proportion_of_profit_to_bank_wallet,
      p.get_bank_wallet_euros(),
      p.get_shop_wallet_euros(),
      today
    );
    const auto expected_bank_wallet{30.0};
    const auto expected_shop_wallet{10.0};
    const auto observed_bank_wallet = p.get_bank_wallet_euros().get_value_euros();
    const auto observed_shop_wallet = p.get_shop_wallet_euros().get_value_euros();
    assert(expected_bank_wallet == observed_bank_wallet);
    assert(expected_shop_wallet == observed_shop_wallet);
  }
}
#endif

bool ribi::imcw::operator==(const person& lhs, const person& rhs) noexcept
{
  return lhs.m_id == rhs.m_id;
}

std::ostream& ribi::imcw::operator<<(std::ostream& os, const person& p) noexcept
{
  std::stringstream s;
  s
    << "Name: " << p.m_name << '\n'
    << "ID: " << p.m_id << '\n'
    << "Auto buy: " << p.m_auto_buy << '\n'
    << "Balance: " << p.m_balance_euros << '\n'
    << "BankWallet: " << p.m_bank_wallet_euros << '\n'
    << "ShopWallet: " << p.m_shop_wallet_euros << '\n'
    << "ClickCard: " << (p.has_click_card() ? "Y" : "N") << '\n'
    << "#Winners: " << p.m_winners.size() << '\n'
  ;
  for (const auto w: p.m_winners) s << w << '\n';
  std::string t{s.str()};
  t.pop_back();
  os << t;
  return os;
}

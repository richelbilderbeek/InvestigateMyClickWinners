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
#include "money.h"
#include "person.h"
#include "winner.h"
#include "winner_package.h"

int ribi::imcw::person::sm_current_id = 0;

ribi::imcw::person::person(
  const std::string& name
) noexcept
  :
    m_auto_buy{true},
    m_balance{"Personal balance of " + name,0.0},
    m_bank_wallet{"BankWallet of " + name,0.0},
    m_card{},
    m_id{sm_current_id++},
    m_name{name},
    m_shop_wallet{"ShopWallet of " + name,0.0},
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

bool ribi::imcw::person::has_account(const balance& an_account) const noexcept
{
  return
       an_account == m_balance
    || an_account == m_bank_wallet
    || an_account == m_shop_wallet
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
    [](const winner& w) { return !w.is_full(); }
  );

  //Convert the Winners that will be converted to BankWallet and ShopWallet
  std::for_each(
    iter,
    std::end(m_winners),
    [this,&the_bank,the_calendar](auto& w)
    {
      assert(w.get_value() >= money(winner::max_value_euros));
      const auto money_before = w.get_value();
      the_bank.transfer(
        w.get_balance(),
        money(euros_from_full_winner_to_shopwallet),
        this->m_shop_wallet,
        the_calendar.get_today()
      );
      assert(w.get_value() > money(0.0));
      assert(w.get_value() > money(40.0));
      const auto money_after = w.get_value();
      assert(money_after < money_before);
      const money value_winner = w.get_value();
      //Transfer rest from Winner to BankWallet
      the_bank.transfer(
        w.get_balance(),
        value_winner,
        this->m_bank_wallet,
        the_calendar.get_today()
      );
      assert(w.get_value() == money(0.0));
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
    while (m_bank_wallet.get_value() >= money(winner::price_vat_exempt_euros))
    {
      const auto bank_wallet_before = m_bank_wallet.get_value();
      the_company.buy_winner(*this,m_bank_wallet,the_bank,the_calendar);
      const auto bank_wallet_after = m_bank_wallet.get_value();
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
    const money expected_euros(0.0);
    const auto observed = p.get_balance().get_value();
    assert(expected_euros == observed);
  }
  //A person buying a starter winner package has to pay 100 euros
  {
    bank b;
    calendar c;
    person p("Mrs B");
    company mcw;
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,c);
    const money expected{-100.0};
    const auto observed = p.get_balance().get_value();
    assert(expected == observed);
  }
  //A person buying an executive winner package has to pay 60+(50*40) euros
  {
    bank b;
    calendar c;
    person p("Mrs C");
    company mwc;
    mwc.buy_winner_package(p,winner_package_name::executive,p.get_balance(),b,c);
    const money expected{-2060.0};
    const auto observed = p.get_balance().get_value();
    assert(expected == observed);
  }
  //A person his/her profit is distributed over BankWallet and ShopWallet
  {
    bank b;
    calendar c;
    person p("Mrs D");
    balance sender("test");
    const money profit_euros(40.0);
    b.transfer(
      sender,
      profit_euros,
      ribi::imcw::person::proportion_of_profit_to_bank_wallet,
      p.get_bank_wallet(),
      p.get_shop_wallet(),
      today
    );
    const money expected_bank_wallet{30.0};
    const money expected_shop_wallet{10.0};
    const auto observed_bank_wallet = p.get_bank_wallet().get_value();
    const auto observed_shop_wallet = p.get_shop_wallet().get_value();
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
    << "Name: " << p.m_name << " (ID: " << p.m_id << ")\n"
    << "Auto buy: " << (p.m_auto_buy ? 'Y' : 'N') << '\n'
    << "Balance: " << p.m_balance << '\n'
    << "BankWallet: " << p.m_bank_wallet << '\n'
    << "ShopWallet: " << p.m_shop_wallet << '\n'
    << "ClickCard: " << (p.has_click_card() ? 'Y' : 'N') << '\n'
    << "#Winners: " << p.m_winners.size() << '\n'
  ;
  for (const winner& w: p.m_winners) {
    s << w << '\n';
  }
  std::string t{s.str()};
  assert(!t.empty());
  t.pop_back();
  os << t;
  return os;
}

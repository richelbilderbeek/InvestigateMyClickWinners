#include "person.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>

#include "buy_winners_strategy.h"
#include "bank.h"
#include "calendar.h"
#include "company.h"
#include "helper.h"
#include "money.h"
#include "person.h"
#include "transfer_strategy.h"
#include "winner.h"
#include "winner_package.h"

int ribi::imcw::person::sm_current_id = 0;

ribi::imcw::person::person(
  const std::string& name,
  const date& end_date
) noexcept
  :
    m_buy_winners_strategy{always_buy()},
    m_balance{"Personal balance of " + name},
    m_bank_wallet{"BankWallet of " + name},
    m_click_cards{},
    m_end_date{end_date},
    m_id{sm_current_id++},
    m_name{name},
    m_shop_wallet{"ShopWallet of " + name},
    m_tranfer_strategy{after_expiration_of_click_card()},
    m_winners{}
{
  #ifndef NDEBUG
  test();
  #endif
  assert(will_buy_winners(boost::gregorian::day_clock::local_day()) == true
      || will_buy_winners(boost::gregorian::day_clock::local_day()) == false
  );
  assert(m_end_date > boost::gregorian::day_clock::local_day());
}

void ribi::imcw::person::add_click_card(const click_card& w)
{
  m_click_cards.push_back(w);
}


void ribi::imcw::person::add_winner(const winner& w)
{
  m_winners.push_back(w);
}

void ribi::imcw::person::buy_click_card(
  bank& the_bank,
  calendar& the_calendar,
  company& the_company
)
{
  assert(!has_valid_click_card(the_calendar.get_today()));
  assert(will_buy_click_card(the_calendar.get_today()));
  the_company.buy_click_card(
    *this,
    m_balance,
    the_bank,
    the_calendar
  );
  //The ClickCard will be valid next month
  assert(!has_valid_click_card(the_calendar.get_today()));
  assert(has_valid_click_card(
      the_calendar.get_today() + boost::gregorian::months(1)
    )
  );
}

bool ribi::imcw::person::has_account(const balance& an_account) const noexcept
{
  return
       an_account == m_balance
    || an_account == m_bank_wallet
    || an_account == m_shop_wallet
  ;
}

bool ribi::imcw::person::has_valid_click_card(const date& d) const noexcept
{
  if (m_click_cards.empty()) return false;
  return m_click_cards.back().is_valid(d);
}

void ribi::imcw::person::process_winners(
  bank& the_bank,
  calendar& the_calendar,
  company& the_company
)
{
  const boost::gregorian::date the_day = the_calendar.get_today();

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
    [this,&the_bank,the_calendar](winner& w)
    {
      assert(w.get_value() >= money(winner::max_value_euros));
      #ifndef NDEBUG
      const auto money_before = w.get_value();
      #endif
      the_bank.transfer(
        w.get_balance(),
        money(euros_from_full_winner_to_shopwallet),
        this->m_shop_wallet,
        the_calendar.get_today()
      );
      assert(w.get_value() > money(0.0));
      assert(w.get_value() > money(40.0));

      #ifndef NDEBUG
      const auto money_after = w.get_value();
      #endif
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
      [](const winner& w) { return w.is_full(); }
    ) == 0
  );

  if (will_buy_winners(the_calendar.get_today()))
  {
    while (m_bank_wallet.get_value() >= money(winner::price_vat_exempt_euros))
    {
      #ifndef NDEBUG
      const auto bank_wallet_before = m_bank_wallet.get_value();
      #endif

      the_company.buy_winner(*this,m_bank_wallet,the_bank,the_day);

      #ifndef NDEBUG
      const auto bank_wallet_after = m_bank_wallet.get_value();
      assert(bank_wallet_after < bank_wallet_before);
      #endif
    }
  }
}

void ribi::imcw::person::remove_card()
{
  if (m_click_cards.empty())
  {
    std::stringstream s;
    s << __func__
      << "Cannot remove a ClickCard when customer has none";
    throw std::logic_error(s.str());
  }
  assert(m_click_cards.size() == 1);
  m_click_cards.pop_back();
}

void ribi::imcw::person::set_winner_buy_strategy(
  const std::function<bool(const date&)>& auto_buy
) noexcept
{
  m_buy_winners_strategy = auto_buy;
  assert(will_buy_winners(boost::gregorian::day_clock::local_day()) == true
      || will_buy_winners(boost::gregorian::day_clock::local_day()) == false
  );
}

#ifndef NDEBUG
void ribi::imcw::person::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }
  using boost::gregorian::months;
  using boost::gregorian::years;
  const helper h;
  const boost::gregorian::date today = boost::gregorian::day_clock::local_day();
  //A person that has not spend anything has a neutral balance
  {
    person p("Mrs A");
    const money expected_euros(0.0);
    const auto observed = p.get_balance().get_value();
    assert(expected_euros == observed);
  }
  //A default person will always buy Winners
  {
    person p("Always buyer");
    p.add_click_card(click_card(today)); //Cannot buy Winners without ClickCard
    assert(p.will_buy_winners(today));
    assert(!p.will_buy_winners(today + months(13)));
    p.add_click_card(click_card(today + years(1))); //Cannot buy Winners without ClickCard
    assert(p.will_buy_winners(today + months(13)));
  }
  //A person that will always buy Winners and is set to do so
  {
    person p("Always buyer");
    p.add_click_card(click_card(today)); //Cannot buy Winners without ClickCard
    p.set_winner_buy_strategy(always_buy());
    assert(p.will_buy_winners(today));
    assert(p.will_buy_winners(today + months(11)));
    assert(!p.will_buy_winners(today + months(13)));
    p.add_click_card(click_card(today + years(1))); //Cannot buy Winners without ClickCard
    assert(p.will_buy_winners(today + months(13)));
  }
  //A person that will never buy Winners
  {
    person p("Never buyer");
    p.add_click_card(click_card(today)); //Cannot buy Winners without ClickCard
    p.set_winner_buy_strategy(never_buy());
    assert(!p.will_buy_winners(today));
    assert(!p.will_buy_winners(today + months(11)));
    assert(!p.will_buy_winners(today + months(13)));
    p.add_click_card(click_card(today + years(1))); //Cannot buy Winners without ClickCard
    assert(!p.will_buy_winners(today + months(13)));
  }
  //A person that buy Winners until a certain date
  {
    person p("Until buyer");
    p.add_click_card(click_card(today)); //Cannot buy Winners without ClickCard
    p.set_winner_buy_strategy(buy_until(today + boost::gregorian::years(1)));
    assert(p.will_buy_winners(today));
    assert(p.will_buy_winners(today + boost::gregorian::months(11)));
    assert(!p.will_buy_winners(today + boost::gregorian::months(13)));
    p.add_click_card(click_card(today + years(1))); //Cannot buy Winners without ClickCard
    assert(!p.will_buy_winners(today + boost::gregorian::months(13)));
  }
  //A person buying any WinnerPackage will obtain a ClickCard that is valid for a year
  {
    bank b;
    calendar c;
    person p("Mrs B");
    company mcw;
    assert(!p.has_valid_click_card(c.get_today()));
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,today);
    assert( p.has_valid_click_card(c.get_today()));
    assert( p.has_valid_click_card(c.get_today() + boost::gregorian::months(11)));
    assert(!p.has_valid_click_card(c.get_today() + boost::gregorian::months(13)));
  }

  //A default person will tranfer his/her money from BankWallet to personal
  //bank account after the expiration date of the ClickCard
  //and when all Winners are gone
  {
    bank b;
    calendar c;
    person p("Mrs B");
    company mcw;
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,c.get_today());
    assert(!p.will_tranfer(c.get_today()));
    assert(!p.will_tranfer(c.get_today() + boost::gregorian::years(1)));
    //Still has winners
    assert(!p.will_tranfer(c.get_today() + boost::gregorian::years(1) + boost::gregorian::days(1)));
    p.get_winners().clear();
    #ifdef FIX_ISSUE_12
    assert(!p.will_tranfer(c.get_today() + boost::gregorian::years(1) + boost::gregorian::days(1)));
    #endif // FIX_ISSUE_12
  }
  //A person buying a starter winner package has to pay 100 euros
  {
    bank b;
    calendar c;
    person p("Mrs B");
    company mcw;
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,c.get_today());
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
    mwc.buy_winner_package(p,winner_package_name::executive,p.get_balance(),b,c.get_today());
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

void ribi::imcw::person::transfer(bank& b, calendar& c)
{
  #ifndef NDEBUG
  const auto balance_before = m_balance.get_value();
  #endif

  const money all_bank_wallet_money = m_bank_wallet.get_value();
  b.transfer(
    m_bank_wallet,
    all_bank_wallet_money,
    m_balance,
    c.get_today()
  );

  #ifndef NDEBUG
  const auto balance_after = m_balance.get_value();
  #endif
  assert(balance_after >= balance_before);
  assert(m_bank_wallet.get_value() == money(0.0));
}

bool ribi::imcw::person::will_buy_click_card(const date& d) const noexcept
{
  using boost::gregorian::months;
  if (has_valid_click_card(d)) { return false; }
  if (m_click_cards.empty()) { return true; }
  if (m_click_cards.back().is_valid(
    d + boost::gregorian::months(2))
  ) {
    return false;
  }
  return d < m_end_date;
}

bool ribi::imcw::person::will_buy_winners(const date& d) const noexcept
{
  if (!has_valid_click_card(d)) return false;
  return m_buy_winners_strategy(d);
}

bool ribi::imcw::person::will_tranfer(const date& d) const noexcept
{
  return m_tranfer_strategy(*this,d);
}


bool ribi::imcw::operator==(const person& lhs, const person& rhs) noexcept
{
  return lhs.m_id == rhs.m_id;
}

std::ostream& ribi::imcw::operator<<(std::ostream& os, const person& p) noexcept
{
  std::stringstream s;
  s
    << "Name: " << p.m_name << " (ID: " << p.m_id << ")\n"
    << "Auto buy: " << (p.m_buy_winners_strategy ? 'Y' : 'N') << '\n'
    << "Balance: " << p.m_balance << '\n'
    << "BankWallet: " << p.m_bank_wallet << '\n'
    << "ShopWallet: " << p.m_shop_wallet << '\n'
  ;
  if (p.m_click_cards.empty()) {
    s << "ClickCard: none" << '\n';
  } else {
    assert(!p.m_click_cards.empty());
    s << p.m_click_cards.back() << '\n';
  }
  s
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

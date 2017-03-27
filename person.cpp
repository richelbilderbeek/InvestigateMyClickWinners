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
  const date& end_date,
  const winner_package_name package
) noexcept
  :
    m_buy_winners_strategy{always_buy()},
    m_balance{"Personal balance of " + name},
    m_bank_wallet{"BankWallet of " + name},
    m_click_cards{},
    m_end_date{end_date},
    m_id{sm_current_id++},
    m_name{name},
    m_package{package},
    m_shop_wallet{"ShopWallet of " + name},
    m_tranfer_strategy{after_expiration_of_click_card()},
    m_winners{}
{
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

bool ribi::imcw::person::has_active_winners(const date& d) const noexcept
{
  if (m_click_cards.empty()) return false;
  const click_card& first_card = m_click_cards[0];
  ///Winners get active the first day of the next month after
  ///the purchase of a ClickCard
  if (d < first_card.get_start_date() + boost::gregorian::months(1)) { return false; }
  const auto cnt = std::count_if(
    std::begin(m_click_cards), std::end(m_click_cards),
    [d](const click_card& c) { return c.is_valid(d); }
  );
  return cnt > 0;
}

bool ribi::imcw::person::has_valid_click_card(const date& d) const noexcept
{
  for (const auto c: m_click_cards) {
    if (c.is_valid(d)) return true;
  }
  return false;
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

#include "person.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>

#include "company.h"
#include "helper.h"
#include "winner_package.h"

int person::sm_current_id = 0;

person::person()
  :
    m_auto_buy{true},
    m_balance_euros{0.0},
    m_bank_wallet_euros{0.0},
    m_card{},
    m_id{sm_current_id++},
    m_shop_wallet_euros{0.0},
    m_winners{}
{
  #ifndef NDEBUG
  test();
  #endif
}

void person::add_click_card(const click_card& w)
{
  assert(!has_click_card());
  m_card.push_back(w);
  assert(has_click_card());
}


void person::add_winner(const winner& w)
{
  m_winners.push_back(w);
}

void person::give_income(const double money_euros) noexcept
{
  const double prev_money = m_bank_wallet_euros + m_shop_wallet_euros;
  m_bank_wallet_euros += (proportion_of_profit_to_bank_wallet * money_euros);
  m_shop_wallet_euros += (proportion_of_profit_to_shop_wallet * money_euros);

  const double new_money = m_bank_wallet_euros + m_shop_wallet_euros;
  assert(prev_money + money_euros == new_money);

}

void person::process_winners(company& the_company)
{
  //Partition is Winners that will remain
  //and Winners that will be converted to BankWallet and ShopWallet
  const auto iter = std::partition(
    std::begin(m_winners),
    std::end(m_winners),
    [](const auto& w)
    {
      return w.get_value_euros() < w.max_value_euros;
    }
  );
  //Convert the Winners that will be converted to BankWallet and ShopWallet
  std::for_each(
    iter,
    std::end(m_winners),
    [this](const auto& w)
    {
      assert(w.get_value_euros() >= w.max_value_euros);
      m_shop_wallet_euros += 2.50;
      m_bank_wallet_euros += w.get_value_euros() - 2.50;
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
    while (m_bank_wallet_euros >= winner::price_vat_exempt_euros)
    {
      const double bank_wallet_before = m_bank_wallet_euros;
      the_company.buy_winner(*this,m_bank_wallet_euros);
      const double bank_wallet_after = m_bank_wallet_euros;
      assert(bank_wallet_after < bank_wallet_before);
    }
  }
}

void person::remove_card()
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
void person::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }
  const helper h;
  //A person that has not spend anything has a neutral balance
  {
    person p;
    const double expected = 0.0;
    const double observed = p.get_balance_euros();
    assert(h.is_about_equal(expected,observed));
  }
  //A person buying a starter winner package has to pay 100 euros
  {
    person p;
    company c;
    c.buy_winner_package(p,winner_package_name::starter,p.get_balance_euros());
    const double expected = -100.0;
    const double observed = p.get_balance_euros();
    assert(h.is_about_equal(expected,observed));
  }
  //A person buying an executive winner package has to pay 60+(50*40) euros
  {
    person p;
    company c;
    c.buy_winner_package(p,winner_package_name::executive,p.get_balance_euros());
    const double expected = -2060.0;
    const double observed = p.get_balance_euros();
    assert(h.is_about_equal(expected,observed));
  }
  //A person his/her profit is distributed over BankWallet and ShopWallet
  {
    person p;
    p.give_income(40.0);
    const double expected_bank_wallet = 30.0;
    const double expected_shop_wallet = 10.0;
    const double observed_bank_wallet = p.get_bank_wallet_euros();
    const double observed_shop_wallet = p.get_shop_wallet_euros();
    assert(h.is_about_equal(expected_bank_wallet,observed_bank_wallet));
    assert(h.is_about_equal(expected_shop_wallet,observed_shop_wallet));
  }
}
#endif

bool operator==(const person& lhs, const person& rhs) noexcept
{
  return lhs.m_id == rhs.m_id;
}

#include "person.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>

#include "company.h"
#include "winner_package.h"

person::person()
  :
    m_balance_euros{0.0},
    m_bank_wallet_euros{0.0},
    m_card{},
    m_shop_wallet_euros{0.0},
    m_winners{}
{
  #ifndef NDEBUG
  test();
  #endif
}

void person::give_income(const double money_euros) noexcept
{
  const double prev_money = m_bank_wallet_euros + m_shop_wallet_euros;
  m_bank_wallet_euros += (proportion_of_profit_to_bank_wallet * money_euros);
  m_shop_wallet_euros += (proportion_of_profit_to_shop_wallet * money_euros);

  const double new_money = m_bank_wallet_euros + m_shop_wallet_euros;
  assert(prev_money + money_euros == new_money);

}

void person::pay(const click_card& c)
{
  if (has_click_card())
  {
    std::stringstream s;
    s << __func__
      << "Cannot buy a ClickCard when customer already has one";
    throw std::logic_error(s.str());
  }
  m_card.push_back(c);
  m_balance_euros -= c.cost_inc_vat_euros;
}

void person::pay(std::shared_ptr<winner> w)
{
  if (!has_click_card())
  {
    std::stringstream s;
    s << __func__
      << "Cannot buy a Winner when customer has no ClickCard";
    throw std::logic_error(s.str());
  }
  m_winners.push_back(w);
  m_balance_euros -= winner::prive_vat_exempt_euros;
}

#ifndef NDEBUG
void person::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }
  //A person that has not spend anything has a neutral balance
  {
    person p;
    const double expected = 0.0;
    const double observed = p.get_balance_euros();
    assert(std::abs(expected - observed) < 0.005);
  }
  //A person buying a starter winner package has to pay 100 euros
  {
    person p;
    company c;
    c.buy(p,winner_package_name::starter);
    const double expected = -100.0;
    const double observed = p.get_balance_euros();
    assert(std::abs(expected - observed) < 0.005);
  }
  //A person buying an executive winner package has to pay 60+(50*40) euros
  {
    person p;
    company c;
    c.buy(p,winner_package_name::executive);
    const double expected = -2060.0;
    const double observed = p.get_balance_euros();
    assert(std::abs(expected - observed) < 0.005);
  }
  //A person his/her profit is distributed over BankWallet and ShopWallet
  {
    person p;
    p.give_income(40.0);
    const double expected_bank_wallet = 30.0;
    const double expected_shop_wallet = 10.0;
    const double observed_bank_wallet = p.get_bank_wallet_euros();
    const double observed_shop_wallet = p.get_shop_wallet_euros();
    assert(std::abs(expected_bank_wallet - observed_bank_wallet) < 0.005);
    assert(std::abs(expected_shop_wallet - observed_shop_wallet) < 0.005);
  }
}
#endif

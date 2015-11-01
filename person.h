#ifndef PERSON_H
#define PERSON_H

#include <vector>

#include "clickcard.h"
#include "winner.h"
#include "winner_package_name.h"

struct person
{
  person();

  void buy(const winner_package_name name);

  ///The amount of money in the person his/her MyClickWinners BankWallet
  ///Cannot be negative
  double get_bank_wallet_euros() const noexcept { return m_bank_wallet_euros; }

  ///The amount of money in the person his/her MyClickWinners ShopWallet
  ///Cannot be negative
  double get_shop_wallet_euros() const noexcept { return m_shop_wallet_euros; }

  double get_balance_euros() const noexcept { return m_balance_euros; }

  ///Give money from MyClickWinners profit
  ///This money is distributed over the BankWallet and ShopWallet
  void give_income(const double money_euros) noexcept;

  constexpr static const double proportion_of_profit_to_bank_wallet = 0.75;
  constexpr static const double proportion_of_profit_to_shop_wallet = 0.25;

  private:

  ///The amount of money in euros in possession of the person,
  ///that is, on his/her personal bank account
  ///Before buying something, this is zero
  ///Positive values denote profit
  ///Negative values denote loss
  double m_balance_euros;

  ///The amount of money in the person his/her MyClickWinners BankWallet
  ///Cannot be negative
  double m_bank_wallet_euros;

  ///A person can have or have not one card
  std::vector<click_card> m_card;


  ///The amount of money in the person his/her MyClickWinners ShopWallet
  ///Cannot be negative
  double m_shop_wallet_euros;


  ///The winners
  std::vector<winner> m_winners;

  #ifndef NDEBUG
  static void test() noexcept;
  #endif

  static_assert(proportion_of_profit_to_bank_wallet + proportion_of_profit_to_shop_wallet == 1.0,"");
};

#endif // PERSON_H

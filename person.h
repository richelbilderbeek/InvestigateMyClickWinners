#ifndef PERSON_H
#define PERSON_H

#include <memory>
#include <vector>

#include "clickcard.h"
#include "winner.h"
#include "winner_package_name.h"

struct company;

struct person
{
  person();

  void add_click_card(const click_card& w);

  void add_winner(const winner& w);

  ///The amount of money in the person his/her MyClickWinners BankWallet
  ///Cannot be negative
  double get_bank_wallet_euros() const noexcept { return m_bank_wallet_euros; }

  ///The amount of money in the person his/her MyClickWinners ShopWallet
  ///Cannot be negative
  double get_shop_wallet_euros() const noexcept { return m_shop_wallet_euros; }

  const double& get_balance_euros() const noexcept { return m_balance_euros; }
        double& get_balance_euros()       noexcept { return m_balance_euros; }

  int get_n_winners() const noexcept { return static_cast<int>(m_winners.size()); }

  ///The winners
  const std::vector<winner>& get_winners() const noexcept { return m_winners; }
        std::vector<winner>& get_winners()       noexcept { return m_winners; }

  ///Give money from MyClickWinners profit
  ///This money is distributed over the BankWallet and ShopWallet
  void give_income(const double money_euros) noexcept;

  bool has_click_card() const noexcept { return !m_card.empty(); }

  ///person pays for a ClickCard
  void pay(const click_card& c);

  ///After profit of MyClickWinners has been distributed,
  ///Winners my have more than 50 euros
  void process_winners(company& the_company);

  ///Remove the ClickCard, assuming he/she has one
  void remove_card();

  void set_auto_buy(const bool auto_buy) noexcept { m_auto_buy = auto_buy; }

  ///Transfer money from BankWallet to m_balance_euros
  ///First transfer costs a fee of tranfer_from_bank_wallet_first_time_fee_euros
  void transfer(const double money_euros) = delete; //TODO

  constexpr static const double max_tranfer_from_bank_wallet_euros = 2250.0;
  constexpr static const double min_tranfer_from_bank_wallet_euros = 100.0;
  constexpr static const double tranfer_from_bank_wallet_first_time_fee_euros = 1000000000.0; //Unknown
  constexpr static const int max_n_winners = 100;
  constexpr static const double proportion_of_profit_to_bank_wallet = 0.75;
  constexpr static const double proportion_of_profit_to_shop_wallet = 0.25;

  private:

  ///Will the person automatically buy Winners when
  ///the ShopWallet exceeds the price of a Winner?
  bool m_auto_buy;

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

  ///Unique ID
  const int m_id;

  ///The amount of money in the person his/her MyClickWinners ShopWallet
  ///Cannot be negative
  double m_shop_wallet_euros;

  ///The Winners a customer has
  std::vector<winner> m_winners;

  ///The new ID to be assigned
  static int sm_current_id;

  #ifndef NDEBUG
  static void test() noexcept;
  #endif

  static_assert(proportion_of_profit_to_bank_wallet + proportion_of_profit_to_shop_wallet == 1.0,"");

  friend bool operator==(const person& lhs, const person& rhs) noexcept;
};

bool operator==(const person& lhs, const person& rhs) noexcept;

#endif // PERSON_H

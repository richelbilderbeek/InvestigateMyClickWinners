#ifndef RIBI_IMCW_PERSON_H
#define RIBI_IMCW_PERSON_H

#include <memory>
#include <vector>

#include "balance.h"
#include "clickcard.h"
#include "winner.h"
#include "winner_package_name.h"

namespace ribi {
namespace imcw {

struct bank;
struct company;
struct calendar;

class person
{
public:
  person(const std::string& name) noexcept;

  void add_click_card(const click_card& w);

  void add_winner(const winner& w);

  ///The amount of money in the person his/her MyClickWinners BankWallet
  ///Cannot be negative
  const balance& get_bank_wallet() const noexcept { return m_bank_wallet; }
        balance& get_bank_wallet()       noexcept { return m_bank_wallet; }

  ///The amount of money in the person his/her MyClickWinners ShopWallet
  ///Cannot be negative
  const balance& get_shop_wallet() const noexcept { return m_shop_wallet; }
        balance& get_shop_wallet()       noexcept { return m_shop_wallet; }

  const balance& get_balance() const noexcept { return m_balance; }
        balance& get_balance()       noexcept { return m_balance; }


  int get_id() const noexcept { return m_id; }

  const std::string& get_name() const noexcept { return m_name; }

  int get_n_winners() const noexcept { return static_cast<int>(m_winners.size()); }

  ///The winners
  const std::vector<winner>& get_winners() const noexcept { return m_winners; }
        std::vector<winner>& get_winners()       noexcept { return m_winners; }

  ///Is this balance an account of the person?
  bool has_account(const balance& an_account) const noexcept;

  bool has_click_card() const noexcept { return !m_card.empty(); }

  ///person pays for a ClickCard
  void pay(const click_card& c);

  ///After profit of MyClickWinners has been distributed,
  ///Winners my have more than 50 euros
  void process_winners(
    bank& the_bank,
    calendar& the_calendar,
    company& the_company
  );

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
  constexpr static const double euros_from_full_winner_to_shopwallet = 2.50;

private:

  ///Will the person automatically buy Winners when
  ///the ShopWallet exceeds the price of a Winner?
  bool m_auto_buy;

  ///The amount of money in euros in possession of the person,
  ///that is, on his/her personal bank account
  ///Before buying something, this is zero
  ///Positive values denote profit
  ///Negative values denote loss
  balance m_balance;

  ///The amount of money in the person his/her MyClickWinners BankWallet
  ///Cannot be negative
  balance m_bank_wallet;

  ///A person can have or have not one card
  std::vector<click_card> m_card;

  ///Unique ID
  const int m_id;

  std::string m_name;

  ///The amount of money in the person his/her MyClickWinners ShopWallet
  ///Cannot be negative
  balance m_shop_wallet;

  ///The Winners a customer has
  std::vector<winner> m_winners;

  ///The new ID to be assigned
  static int sm_current_id;

  #ifndef NDEBUG
  static void test() noexcept;
  #endif

  static_assert(proportion_of_profit_to_bank_wallet + proportion_of_profit_to_shop_wallet == 1.0,"");

  friend std::ostream& operator<<(std::ostream& os, const person& p) noexcept;
  friend bool operator==(const person& lhs, const person& rhs) noexcept;
};


std::ostream& operator<<(std::ostream& os, const person& p) noexcept;

bool operator==(const person& lhs, const person& rhs) noexcept;

} //~namespace imcw
} //~namespace ribi

#endif // RIBI_IMCW_PERSON_H

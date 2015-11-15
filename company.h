#ifndef RIBI_IMCW_COMPANY_H
#define RIBI_IMCW_COMPANY_H

#include <memory>
#include <vector>
#include "balance.h"
#include "person.h"

namespace ribi {
namespace imcw {

class bank;

///The company MyClickWinners
struct company
{
  using Customer = std::reference_wrapper<person>;
  using Customers = std::vector<Customer>;

  company();

  ///Add any customer, these persons are setup in the simulation
  void add(person& customer);

  #ifndef NDEBUG
  ///Ban any customer, removing his/her clickcard
  void ban(const person& customer);
  #endif

  ///A customer buys a ClickCard
  ///This money is added to the balance of undistributed money
  void buy_click_card(
    person& customer,
    balance& customer_balance,
    bank& the_bank,
    calendar& the_calendar
  );

  ///A customer buys Winner
  ///This money is added to the balance of undistributed money
  void buy_winner(
    person& customer,
    balance& customer_balance,
    bank& the_bank,
    calendar& the_calendar
  );

  ///A customer buys a WinnerPackage
  ///This money is added to the balance of undistributed money
  void buy_winner_package(
    person& customer,
    const winner_package_name name,
    balance& customer_balance,
    bank& the_bank,
    calendar& the_calendar
  );

  //int count_winners() noexcept;
  int count_active_customers() const noexcept { return 0; }

  ///When MyClickWinners makes a profit,
  ///it is distributed over customers and other entities
  /// * compensation_plan: 15%
  /// * holding: 10%
  /// * reserves: 30%
  /// * winners: 45%
  void distribute_net_profit(
    bank& the_bank,
    calendar& the_calendar
  ) noexcept;

  ///The balance of the compensation plan of MyClickWinners
  ///Positive values denote there is a money available
  ///Negative values denote there is a money shortage
  const balance& get_balance_compensation_plan() const noexcept { return m_balance_compensation_plan; }

  ///The balance of the holding of MyClickWinners
  ///Positive values denote there is a money available
  ///Negative values denote there is a money shortage
  const balance& get_balance_holding() const noexcept { return m_balance_holding; }

  ///The balance of the reserves of MyClickWinners
  ///Positive values denote there is a money available
  ///Negative values denote there is a money shortage
  const balance& get_balance_reserves() const noexcept { return m_balance_reserves; }

  ///The balance of the money that is not yet distributed,
  ///for example, when a customer has just bought a WinnerPackage,
  ///but before distribution of the net profit
  ///Positive values denote there is a money available
  ///Negative values denote there is a money shortage
  const balance& get_balance_undistributed() const noexcept { return m_balance_undistributed; }

  const Customers& get_customers() const noexcept { return m_customers; }
        Customers& get_customers()       noexcept { return m_customers; }

  ///Get the number of Winners distributed
  //int get_n_winners() const noexcept { return static_cast<int>(m_winners.size()); }

  ///Is the person already a customer?
  bool is_customer(const person& p) const noexcept;

  ///Move money from source to 'undistributed' balance
  void transfer(
    balance& source,
    bank& the_bank,
    calendar& the_calendar
  ) noexcept;

  constexpr static const double proportion_of_profit_to_compensation_plan = 0.15;
  constexpr static const double proportion_of_profit_to_holding = 0.10;
  constexpr static const double proportion_of_profit_to_reserves = 0.30;
  constexpr static const double proportion_of_profit_to_winners = 0.45;

  private:

  ///The balance of the compensation plan of MyClickWinners
  ///Positive values denote there is a money available
  ///Negative values denote there is a money shortage
  balance m_balance_compensation_plan;

  ///The balance of the holding of MyClickWinners
  ///Positive values denote there is a money available
  ///Negative values denote there is a money shortage
  balance m_balance_holding;

  ///The balance of the reserves of MyClickWinners
  ///Positive values denote there is a money available
  ///Negative values denote there is a money shortage
  balance m_balance_reserves;

  ///The balance of the money that is not yet distributed,
  ///for example, when a customer has just bought a WinnerPackage,
  ///but before distribution of the net profit
  ///Positive values denote there is a money available
  ///Negative values denote there is a money shortage
  balance m_balance_undistributed;

  ///All customers
  Customers m_customers;

  bool m_verbose;

  ///Collect all Winners from all customers
  std::vector<std::reference_wrapper<winner>> collect_winners() noexcept;

  ///Distribute the net profit of the Winners
  void distribute_net_profit_winners(
    balance& source,
    const money& total_money,
    bank& the_bank,
    calendar& the_calendar
  ) noexcept;

  ///Distribute the net profit of the Winners
  void distribute_net_profit_compensation_plan(
    balance& source,
    const money& total_money,
    bank& the_bank,
    calendar& the_calendar
  ) noexcept;

  #ifndef NDEBUG
  static void test() noexcept;
  #endif

  static_assert(
    proportion_of_profit_to_compensation_plan
    + proportion_of_profit_to_holding
    + proportion_of_profit_to_reserves
    + proportion_of_profit_to_winners
    == 1.0,"Proportions must sum up to one"
  );
  friend std::ostream& operator<<(std::ostream& os, const company& c) noexcept;

};


std::ostream& operator<<(std::ostream& os, const company& c) noexcept;

} //~namespace imcw
} //~namespace ribi

#endif // RIBI_IMCW_COMPANY_H

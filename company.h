#ifndef COMPANY_H
#define COMPANY_H

#include <memory>
#include <vector>
#include "person.h"

///The company MyClickWinners
struct company
{
  company();

  ///A customer buys a WinnerPackage
  ///This money is added to the balance of undistributed money
  void buy(person& customer, const winner_package_name name);

  ///When MyClickWinners makes a profit,
  ///it is distributed over customers and other entities
  void distribute_net_profit(const double money_euros) noexcept;

  ///The balance of the compensation plan of MyClickWinners
  ///Positive values denote there is a money available
  ///Negative values denote there is a money shortage
  double get_balance_compensation_plan_euros() const noexcept { return m_balance_compensation_plan_euros; }

  ///The balance of the holding of MyClickWinners
  ///Positive values denote there is a money available
  ///Negative values denote there is a money shortage
  double get_balance_holding_euros() const noexcept { return m_balance_holding_euros; }

  ///The balance of the reserves of MyClickWinners
  ///Positive values denote there is a money available
  ///Negative values denote there is a money shortage
  double get_balance_reserves_euros() const noexcept { return m_balance_reserves_euros; }

  ///The balance of the money that is not yet distributed,
  ///for example, when a customer has just bought a WinnerPackage,
  ///but before distribution of the net profit
  ///Positive values denote there is a money available
  ///Negative values denote there is a money shortage
  double get_balance_undistributed_euros() const noexcept { return m_balance_undistributed_euros; }

  ///Get the number of Winners distributed
  //int get_n_winners() const noexcept { return static_cast<int>(m_winners.size()); }

  ///Is the person already a customer?
  bool is_customer(const person& p) const noexcept;

  constexpr static const double proportion_of_profit_to_compensation_plan = 0.15;
  constexpr static const double proportion_of_profit_to_holding = 0.10;
  constexpr static const double proportion_of_profit_to_reserves = 0.30;
  constexpr static const double proportion_of_profit_to_winners = 0.45;

  private:

  ///The balance of the compensation plan of MyClickWinners
  ///Positive values denote there is a money available
  ///Negative values denote there is a money shortage
  double m_balance_compensation_plan_euros;

  ///The balance of the holding of MyClickWinners
  ///Positive values denote there is a money available
  ///Negative values denote there is a money shortage
  double m_balance_holding_euros;

  ///The balance of the reserves of MyClickWinners
  ///Positive values denote there is a money available
  ///Negative values denote there is a money shortage
  double m_balance_reserves_euros;

  ///The balance of the money that is not yet distributed,
  ///for example, when a customer has just bought a WinnerPackage,
  ///but before distribution of the net profit
  ///Positive values denote there is a money available
  ///Negative values denote there is a money shortage
  double m_balance_undistributed_euros;

  ///All customers
  std::vector<std::reference_wrapper<person>> m_customers;

  ///Collect all Winners from all customers
  std::vector<std::reference_wrapper<winner>> collect_winners() noexcept;

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

};

#endif // COMPANY_H

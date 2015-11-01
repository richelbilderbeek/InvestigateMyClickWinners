#ifndef COMPANY_H
#define COMPANY_H

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

  ///The customers
  const std::vector<person>& get_customers() const noexcept { return m_customers; }

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

  ///The customers
  std::vector<person> m_customers;

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

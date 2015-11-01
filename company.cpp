#include "company.h"

#include <cmath>
#include <cassert>
#include <sstream>

#include "winner_package.h"
company::company()
  : m_balance_compensation_plan_euros{0.0},
    m_balance_holding_euros{0.0},
    m_balance_reserves_euros{0.0},
    m_balance_undistributed_euros{0.0},
    m_customers{}
{
  #ifndef NDEBUG
  test();
  #endif
}

void company::buy(person& customer, const winner_package_name name)
{
  if (customer.has_click_card())
  {
    std::stringstream s;
    s << __func__
      << "Cannot buy a WinnerPackage when customer already has a ClickCard";
    throw std::logic_error(s.str());
  }
  const winner_package p(name);
  const int n_winners = p.get_n_winners();

  //ClickCard
  {
    click_card c;
    customer.pay(c);
    m_balance_undistributed_euros += c.cost_inc_vat_euros;
  }
  //Winners
  for (int i=0; i!=n_winners; ++i)
  {
    winner w;
    customer.pay(w);
    m_balance_undistributed_euros += w.cost_vat_exempt_euros;
  }

}

#ifndef NDEBUG
void company::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }
  //When a company is started, all balances are zero
  {
    company c;
    assert(c.get_balance_compensation_plan_euros() == 0.0);
    assert(c.get_balance_holding_euros () == 0.0);
    assert(c.get_balance_reserves_euros() == 0.0);
    assert(c.get_customers().empty());
  }
  //When a company is started, all balances are zero
  {
    company c;
    person p;
    c.buy(p,winner_package_name::basic);
    assert(c.get_balance_compensation_plan_euros() == 0.0);
    assert(c.get_balance_holding_euros () == 0.0);
    assert(c.get_balance_reserves_euros() == 0.0);
    assert(c.get_customers().empty());
  }
  //A person buying a starter winner package results in 100 euros in the undistributed money balance
  {
    person p;
    company c;
    c.buy(p,winner_package_name::starter);
    const double expected = 100.0;
    const double observed = c.get_balance_undistributed_euros();
    assert(std::abs(expected - observed) < 0.005);
  }
  //A person buying an executive winner package has to pay 60+(50*40) euros in the undistributed money balance
  {
    person p;
    company c;
    c.buy(p,winner_package_name::executive);
    const double expected = 2060.0;
    const double observed = c.get_balance_undistributed_euros();
    assert(std::abs(expected - observed) < 0.005);
  }
}
#endif

#include "company.h"

#include <cassert>

company::company()
  : m_balance_compensation_plan_euros{0.0},
    m_balance_holding_euros{0.0},
    m_balance_reserves_euros{0.0},
    m_customers{}
{
  #ifndef NDEBUG
  test();
  #endif
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
}
#endif

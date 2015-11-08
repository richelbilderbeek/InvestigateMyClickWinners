#ifndef RIBI_IMCW_SIMULATION_H
#define RIBI_IMCW_SIMULATION_H

#include <vector>
#include "bank.h"
#include "company.h"
#include "calendar.h"
#include "person.h"
#include "simulation_parameters.h"

namespace ribi {
namespace imcw {

///A simulation describes the profit of a focal person,
///compared with other persons
struct simulation
{
  simulation(
    const bank& any_bank,
    const calendar& any_calendar,
    const company& any_company,
    const simulation_parameters& parameters
  );

  void run() noexcept;

  private:
  bank m_bank;

  calendar m_calendar;

  company m_company;

  const simulation_parameters m_parameters;

  #ifndef NDEBUG
  static void test() noexcept;
  #endif
};

} //~namespace imcw
} //~namespace ribi

#endif // RIBI_IMCW_SIMULATION_H

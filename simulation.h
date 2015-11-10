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
    const simulation_parameters& parameters
  );

  const bank& get_bank() const noexcept { return m_bank; }
  const calendar& get_calendar() const noexcept { return m_calendar; }
  const company& get_company() const noexcept { return m_company; }

  void do_timestep() noexcept;

  private:

  bank m_bank;
  calendar m_calendar;
  company m_company;
  person m_focal_person;
  std::vector<person> m_others;
  const simulation_parameters m_parameters;

  #ifndef NDEBUG
  static void test() noexcept;
  #endif
};

} //~namespace imcw
} //~namespace ribi

#endif // RIBI_IMCW_SIMULATION_H

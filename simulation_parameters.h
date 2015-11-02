#ifndef SIMULATION_PARAMETERS_H
#define SIMULATION_PARAMETERS_H

#include <vector>

#include "person.h"

struct simulation_parameters
{
  explicit simulation_parameters(
    const person& me,
    const std::vector<person>& others
  );

  const person& get_focal_person() const noexcept { return m_focal_person; }
  const std::vector<person>&  get_others() const noexcept { return m_others; }

  private:
  const person m_focal_person;
  const std::vector<person> m_others;

  private:
  #ifndef NDEBUG
  static void test() noexcept;
  #endif
};

#endif // SIMULATION_PARAMETERS_H

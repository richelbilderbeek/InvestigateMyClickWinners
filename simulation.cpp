#include "simulation.h"

#include <algorithm>

simulation::simulation(const simulation_parameters& parameters)
  : m_company{},
    m_parameters{parameters}
{

}

void simulation::run() noexcept
{
  company c;
  person focal_person = m_parameters.get_focal_person();
  std::vector<person> others = m_parameters.get_others();
  //Add all people to company
  c.add(focal_person);
  std::for_each(
    std::begin(others),
    std::end(others),
    [&c](auto& p) { c.add(p); }
  );

}

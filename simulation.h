#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include "company.h"
#include "person.h"
#include "simulation_parameters.h"

struct simulation
{
  simulation(const simulation_parameters& parameters);

  void run();

  private:
  company m_company;

  const simulation_parameters m_parameters;

  #ifndef NDEBUG
  static void test() noexcept;
  #endif
};

#endif // SIMULATION_H

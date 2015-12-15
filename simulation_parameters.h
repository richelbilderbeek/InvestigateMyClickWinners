#ifndef RIBI_IMCW_SIMULATION_PARAMETERS_H
#define RIBI_IMCW_SIMULATION_PARAMETERS_H

#include <vector>
#include "fparser.hh"
#include "person.h"
#include <boost/date_time/gregorian/gregorian.hpp>

namespace ribi {
namespace imcw {

struct simulation_parameters
{
  explicit simulation_parameters(
    const person& focal_person,
    const std::vector<person>& others,
    const boost::gregorian::date& start,
    const boost::gregorian::date& end,
    const std::string& profit_webshop_per_year_in_euros_equation,
    const std::string& profit_website_per_month_in_euros_equation,
    const int rng_seed
  );

  const boost::gregorian::date& get_end() const noexcept { return m_end; }
  const person& get_focal_person() const noexcept { return m_focal_person; }
  const std::vector<person>&  get_others() const noexcept { return m_others; }

  ///Get the year webshop profit
  money get_profit_webshop_per_year(
    const boost::gregorian::date& day = boost::gregorian::date(),
    const int n_customers = 0
  ) const noexcept;

  ///Get the monthly website profit
  money get_profit_website_per_month(
    const boost::gregorian::date& day = boost::gregorian::date(),
    const int n_customers = 0
  ) const noexcept;

  int get_rng_seed() const noexcept { return m_rng_seed; }

  const boost::gregorian::date& get_start() const noexcept { return m_start; }

  private:
  const boost::gregorian::date m_end;
  const person m_focal_person;
  const std::vector<person> m_others;

  const FunctionParser m_profit_webshop_per_year;
  const FunctionParser m_profit_website_per_month;

  const int m_rng_seed;

  const boost::gregorian::date m_start;

  private:
  #ifndef NDEBUG
  static void test() noexcept;
  #endif
};

///Check if an equation depending on 't' and 'n' can be parsed
bool can_parse_equation(const std::string& equation) noexcept;

///Creates a parser, iff can_parse_equation is true
///throws otherwise
FunctionParser create_parser(const std::string& equation);

} //~namespace imcw
} //~namespace ribi

#endif // RIBI_IMCW_SIMULATION_PARAMETERS_H

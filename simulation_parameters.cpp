#include "simulation_parameters.h"

#include <cassert>
#include <stdexcept>
#include <sstream>

#include "bank.h"
#include "calendar.h"
#include "fparser.hh"


ribi::imcw::simulation_parameters::simulation_parameters(
  const person& focal_person,
  const std::vector<person>& others,
  const boost::gregorian::date& start,
  const boost::gregorian::date& end,
  const std::string& profit_webshop_per_year_in_euros_equation,
  const std::string& profit_website_per_month_in_euros_equation,
  const int rng_seed
) : m_end{end},
    m_focal_person{focal_person},
    m_others{others},
    m_profit_webshop_per_year{create_parser(profit_webshop_per_year_in_euros_equation)},
    m_profit_website_per_month{create_parser(profit_website_per_month_in_euros_equation)},
    m_rng_seed{rng_seed},
    m_start{start}
{
  if (start >= end)
  {
    std::stringstream msg;
    msg << __func__ << ": start date (" << start
      << ") must be before end date (" << end
      << ")"
    ;
    throw std::logic_error(msg.str());
  }
  assert(m_profit_webshop_per_year.GetParseErrorType() == FunctionParser::FP_NO_ERROR);
  assert(m_profit_website_per_month.GetParseErrorType() == FunctionParser::FP_NO_ERROR);
}

bool ribi::imcw::can_parse_equation(const std::string& equation) noexcept
{
  FunctionParser f;
  f.Parse(equation.c_str(),"t,n");
  return f.GetParseErrorType() == FunctionParser::FP_NO_ERROR;
}

FunctionParser ribi::imcw::create_parser(const std::string& equation)
{
  if (!can_parse_equation(equation)) {
    std::stringstream msg;
    msg
      << __func__ << ": cannot create parser from equation '"
      << equation << "' with variables 't' and 'n'"
    ;
    throw std::logic_error(msg.str());
  }
  FunctionParser f;
  f.Parse(equation.c_str(),"t,n");
  return f;
}


ribi::imcw::money ribi::imcw::simulation_parameters::get_profit_webshop_per_year(
  const boost::gregorian::date& day,
  const int n_customers
) const noexcept
{
  const double t{
    static_cast<double>(day.day_number() - m_start.day_number())
  };
  const double n{
    static_cast<double>(n_customers)
  };

  assert(m_profit_webshop_per_year.GetParseErrorType() == FunctionParser::FP_NO_ERROR);

  //Evaluate the parsed formula
  const double xs[2] = {t, n};

  //FunctionParser::Eval is not const correct, grumble, grumble...
  const double profit_in_euros{
    const_cast<FunctionParser&>(m_profit_webshop_per_year).Eval(xs)
  };

  if (m_profit_webshop_per_year.EvalError())
  {
    std::clog << __func__ << ": function could not be evaluated" << std::endl;
    return money(0.0);
  }

  return money(profit_in_euros);
}

ribi::imcw::money ribi::imcw::simulation_parameters::get_profit_website_per_month(
  const boost::gregorian::date& day,
  const int n_customers
) const noexcept
{
  const double t{
    static_cast<double>(day.day_number() - m_start.day_number())
  };
  const double n{
    static_cast<double>(n_customers)
  };

  assert(m_profit_website_per_month.GetParseErrorType() == FunctionParser::FP_NO_ERROR);

  //Evaluate the parsed formula
  const double xs[2] = {t, n};

  //FunctionParser::Eval is not const correct, grumble, grumble...
  const double profit_in_euros{
    const_cast<FunctionParser&>(m_profit_website_per_month).Eval(xs)
  };

  if (m_profit_website_per_month.EvalError())
  {
    std::clog << __func__ << ": function could not be evaluated" << std::endl;
    return money(0.0);
  }

  return money(profit_in_euros);
}

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
  const money& profit_webshop_per_year,
  const money& profit_website_per_month,
  const int rng_seed
) : m_end{end},
    m_focal_person{focal_person},
    m_others{others},
    m_profit_webshop_per_year{profit_webshop_per_year},
    m_profit_website_per_month{profit_website_per_month},
    m_rng_seed{rng_seed},
    m_start{start}
{
  #ifndef NDEBUG
  test();
  #endif
  if (start >= end)
  {
    std::stringstream msg;
    msg << __func__ << ": start date (" << start
      << ") must be before end date (" << end
      << ")"
    ;
    throw std::logic_error(msg.str());
  }
}


ribi::imcw::money ribi::imcw::simulation_parameters::get_profit_webshop_per_year(
  const boost::gregorian::date& day,
  const int n_customers
) const noexcept
{
  FunctionParser f;

  const double t{
    static_cast<double>(day.day_number())
  };
  const double n{
    static_cast<double>(n_customers)
  };

  //Parse the formula
  std::stringstream my_function;
  my_function
    << m_profit_webshop_per_year.get_value_euros()
    << "+(0.0 * t)"
    << "+(0.0 * n)"
  ;

  f.Parse(my_function.str().c_str(),"t,n");

  assert(f.GetParseErrorType() == FunctionParser::FP_NO_ERROR);

  //Evaluate the parsed formula
  const double xs[2] = {t, n};
  const double y = f.Eval(xs);

  if (f.EvalError())
  {
    std::clog << "Function could not be evaluated" << std::endl;
    return money(0.0);
  }

  return money(y);
}

ribi::imcw::money ribi::imcw::simulation_parameters::get_profit_website_per_month(
  const boost::gregorian::date& day,
  const int n_customers
) const noexcept
{
  FunctionParser f;

  const double d{
    static_cast<double>(day.day_number())
  };
  const double n{
    static_cast<double>(n_customers)
  };

  //Parse the formula
  std::stringstream my_function;
  my_function
    << m_profit_website_per_month.get_value_euros()
    << "+ (0.0 * t)"
    << "+ (0.0 * n)"
  ;

  f.Parse(my_function.str().c_str(),"t,n");

  assert(f.GetParseErrorType() == FunctionParser::FP_NO_ERROR);

  //Evaluate the parsed formula
  const double xs[2] = {d, n};
  const double y = f.Eval(xs);

  if (f.EvalError())
  {
    std::clog << "Function could not be evaluated" << std::endl;
    return money(0.0);
  }

  return money(y);
}



#ifndef NDEBUG
void ribi::imcw::simulation_parameters::test() noexcept
{
  {
    static bool is_tested{false};
    if (is_tested) return;
    is_tested = true;
  }

  //Correct date range
  {
    boost::gregorian::date today = boost::gregorian::day_clock::local_day();
    boost::gregorian::date yesterday = today - boost::gregorian::days(1);
    boost::gregorian::date tomorrow = today + boost::gregorian::days(1);
    bank b;
    calendar c;
    person p("Mrs. A");
    simulation_parameters(p,{},today,tomorrow,money(0.0),money(0.0),0);
    try {
      simulation_parameters(p,{},today,yesterday,money(0.0),money(0.0),0);
      assert(!"Should not get here");
    }
    catch (std::logic_error&) {
      //OK
    }
  }
  //Test the FunctionParser
  {
    FunctionParser f;

    const double t{1.0};
    const double n{1.0};

    //Parse the formula
    std::stringstream my_function;
    my_function
      << "1.0"
      << "+(2.0 * t)"
      << "+(3.0 * n)"
    ;
    f.Parse(my_function.str().c_str(),"t,n");

    assert(f.GetParseErrorType() == FunctionParser::FP_NO_ERROR);

    //Evaluate the parsed formula
    const double xs[2] = {t, n};
    const double y = f.Eval(xs);
    assert(!f.EvalError());
    assert(std::abs(y - 6.0) < 0.001);
  }
  {
    boost::gregorian::date today = boost::gregorian::day_clock::local_day();
    boost::gregorian::date end = today + boost::gregorian::years(1);
    bank b;
    calendar c;
    person p("Mrs. A");
    simulation_parameters parameters{
      p,{},today,end,money(0.0),money(0.0),0
    };
    assert(parameters.get_profit_webshop_per_year() >= money(0.0));
    assert(parameters.get_profit_website_per_month() >= money(0.0));
  }
}
#endif


#include "simulation_parameters.h"

void ribi::imcw::simulation_parameters::test() noexcept
{
  //Correct date range
  {
    boost::gregorian::date today = boost::gregorian::day_clock::local_day();
    boost::gregorian::date yesterday = today - boost::gregorian::days(1);
    boost::gregorian::date tomorrow = today + boost::gregorian::days(1);
    bank b;
    calendar c;
    person p("Mrs. A");
    simulation_parameters(p,{},today,tomorrow,"0.0","0.0",0);
    try {
      simulation_parameters(p,{},today,yesterday,"0.0","0.0",0);
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
    assert(can_parse_equation("3.14"));
    assert(can_parse_equation("3.14 * t"));
    assert(can_parse_equation("3.14 * t * n"));
    assert(!can_parse_equation("3.14 * x"));
  }
  //Parameters with more complex function parser arguments
  {
    boost::gregorian::date today = boost::gregorian::day_clock::local_day();
    boost::gregorian::date end = today + boost::gregorian::years(1);
    bank b;
    calendar c;
    person p("Mrs. A");
    simulation_parameters parameters{
      p,
      {},
      today,
      end,
      "1.0 + (1.0 * n) + (1.0 * t)",
      "1.0 + (1.0 * n) + (1.0 * t)",
      42 //RNG
    };
    const int n_others{0};
    assert(parameters.get_profit_webshop_per_year(end,n_others + 1) >= money(0.0));
    assert(parameters.get_profit_website_per_month(end,n_others + 1) >= money(0.0));
  }
}


#include "simulation.h"

void ribi::imcw::simulation::test() noexcept
{
  //const boost::gregorian::date today = boost::gregorian::day_clock::local_day();
  //If a member wants to be member for a year,
  //then after a year, the ClickCard is not valid any more
  //and the balance will be -100 euro
  {
    const boost::gregorian::date d(2015,1,15);
    const auto membership_end = d + boost::gregorian::years(1);
    const auto simulation_end = d + boost::gregorian::months(15);
    const simulation_parameters parameters(
      person("Mister X",membership_end),
      {},
      d,
      simulation_end,
      "0.0",
      "0.0",
      42
    );
    simulation s(parameters);
    while (!s.is_done()) { s.do_timestep(); }
    const auto p = s.get_focal_person();
    assert(!p.has_valid_click_card(
      s.get_calendar().get_today()
      )
    );
    assert(p.get_balance().get_value() == money(-100.0));
  }
  //#define FIX_ISSUE_16
  #ifdef FIX_ISSUE_16
  //When you buy a ClickCard, first income will be at
  //the end of the next month
  {
    const boost::gregorian::date purchase_date(2015,1,15);
    const auto membership_end = purchase_date + boost::gregorian::years(1);
    const auto simulation_end = purchase_date + boost::gregorian::months(15);
    const simulation_parameters parameters(
      person("Mister X",membership_end),
      {},
      purchase_date,
      simulation_end,
      "100.0",
      "100.0",
      42
    );
    simulation s(parameters);
    {
      const auto p = s.get_focal_person();
      assert(!p.has_valid_click_card(s.get_calendar().get_today()));
    }
    for (int i=0; i!=31; ++i) { s.do_timestep(); }
    const auto p = s.get_focal_person();
    assert(p.has_valid_click_card(s.get_calendar().get_today()));
    assert(p.get_bank_wallet().get_value() == money(0.0));
    assert(p.get_shop_wallet().get_value() == money(0.0));
    assert(!"Fixed #16");
  }
  #endif // FIX_ISSUE_16
}

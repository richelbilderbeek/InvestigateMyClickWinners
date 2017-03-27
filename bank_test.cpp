#include "bank.h"

#include <boost/test/unit_test.hpp>

#include "money.h"
#include "balance.h"

using namespace ribi::imcw;

BOOST_AUTO_TEST_CASE(imcw_bank)
{
  //const bool verbose{false};
  const boost::gregorian::date today = boost::gregorian::day_clock::local_day();

  {
    bank b;
  }
  {
    bank the_bank;
    balance sender("sender");
    const money amount_euros(100.0);
    balance receiver("receiver");
    BOOST_CHECK(sender.get_value() == money(0.0));
    BOOST_CHECK(receiver.get_value() == money(0.0));
    BOOST_CHECK(the_bank.get_transfers().empty());
    the_bank.transfer(
      sender,
      amount_euros,
      receiver,
      today
    );
    BOOST_CHECK(sender.get_value()   == -amount_euros);
    BOOST_CHECK(receiver.get_value() ==  amount_euros);
    BOOST_CHECK(!the_bank.get_transfers().empty());
  }
}

#include "bank.h"

void ribi::imcw::bank::test() noexcept
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
    assert(sender.get_value() == money(0.0));
    assert(receiver.get_value() == money(0.0));
    assert(the_bank.get_transfers().empty());
    the_bank.transfer(
      sender,
      amount_euros,
      receiver,
      today
    );
    assert(sender.get_value()   == -amount_euros);
    assert(receiver.get_value() ==  amount_euros);
    assert(!the_bank.get_transfers().empty());
  }
}

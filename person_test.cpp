#include "person.h"

void ribi::imcw::person::test() noexcept
{
  using boost::gregorian::months;
  using boost::gregorian::years;
  const helper h;
  //A person that has not spend anything has a neutral balance
  {
    person p("Mrs A");
    const money expected_euros(0.0);
    const auto observed = p.get_balance().get_value();
    assert(expected_euros == observed);
  }
  //A default person will always buy Winners
  {
    person p("Always buyer");

    p.add_click_card(click_card(date(2015,6,15))); //Cannot buy Winners without ClickCard
    assert(!p.will_buy_winners(date(2015,6,15)));
    assert( p.will_buy_winners(date(2015,7, 1)));
    assert( p.will_buy_winners(date(2016,6,30)));
    assert(!p.will_buy_winners(date(2016,7, 1)));
    p.add_click_card(click_card(date(2016,6,15))); //Cannot buy Winners without ClickCard
    assert( p.will_buy_winners(date(2016,7, 1)));
    assert(!p.will_buy_winners(date(2017,7, 1)));
  }
  //A person that will always buy Winners and is set to do so
  {
    person p("Always buyer");
    p.set_winner_buy_strategy(always_buy());
    p.add_click_card(click_card(date(2015,6,15))); //Cannot buy Winners without ClickCard
    assert(!p.will_buy_winners(date(2015,6,15)));
    assert( p.will_buy_winners(date(2015,7, 1)));
    assert( p.will_buy_winners(date(2016,6,30)));
    assert(!p.will_buy_winners(date(2016,7, 1)));
    p.add_click_card(click_card(date(2016,6,15))); //Cannot buy Winners without ClickCard
    assert( p.will_buy_winners(date(2016,7, 1)));
    assert(!p.will_buy_winners(date(2017,7, 1)));
  }
  //A person that will never buy Winners
  {
    person p("Never buyer");
    p.set_winner_buy_strategy(never_buy());
    p.add_click_card(click_card(date(2015,6,15))); //Cannot buy Winners without ClickCard
    assert(!p.will_buy_winners(date(2015,6,15)));
    assert(!p.will_buy_winners(date(2015,7, 1)));
    assert(!p.will_buy_winners(date(2016,6,30)));
    assert(!p.will_buy_winners(date(2016,7, 1)));
    p.add_click_card(click_card(date(2016,6,15))); //Cannot buy Winners without ClickCard
    assert(!p.will_buy_winners(date(2016,7, 1)));
    assert(!p.will_buy_winners(date(2017,7, 1)));
  }
  //A person that buy Winners until a certain date
  {
    person p("Until buyer");
    p.set_winner_buy_strategy(buy_until(date(2016,2,15)));
    p.add_click_card(click_card(date(2015,6,15))); //Cannot buy Winners without ClickCard
    assert(!p.will_buy_winners(date(2015,6,15)));
    assert( p.will_buy_winners(date(2015,7, 1)));
    assert(!p.will_buy_winners(date(2016,6,30)));
    assert(!p.will_buy_winners(date(2016,7, 1)));
    p.add_click_card(click_card(date(2016,6,15))); //Cannot buy Winners without ClickCard
    assert(!p.will_buy_winners(date(2016,7, 1)));
    assert(!p.will_buy_winners(date(2017,7, 1)));
  }
  //A person buying any WinnerPackage will obtain a ClickCard that is valid for a year
  {
    bank b;
    calendar c;
    person p("Mrs B");
    company mcw;
    ;
    assert(!p.has_valid_click_card(c.get_today()));
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,date(2015,6,15));
    assert(!p.has_valid_click_card(date(2015,6,15)));
    assert( p.has_valid_click_card(date(2015,7, 1)));
    assert( p.has_valid_click_card(date(2016,6,30)));
    assert(!p.has_valid_click_card(date(2016,7, 1)));
  }

  //A default person will tranfer his/her money from BankWallet to personal
  //bank account after the expiration date of the ClickCard
  //and when all Winners are gone
  {
    bank b;
    calendar c;
    person p("Mrs B");
    company mcw;
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,c.get_today());
    assert(!p.will_tranfer(c.get_today()));
    assert(!p.will_tranfer(c.get_today() + boost::gregorian::years(1)));
    //Still has winners
    assert(!p.will_tranfer(c.get_today() + boost::gregorian::years(1) + boost::gregorian::days(1)));
    p.get_winners().clear();
    #ifdef FIX_ISSUE_12
    assert(!p.will_tranfer(c.get_today() + boost::gregorian::years(1) + boost::gregorian::days(1)));
    #endif // FIX_ISSUE_12
  }
  //A person buying a starter winner package has to pay 100 euros
  {
    bank b;
    calendar c;
    person p("Mrs B");
    company mcw;
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,c.get_today());
    const money expected{-100.0};
    const auto observed = p.get_balance().get_value();
    assert(expected == observed);
  }
  //A person buying an executive winner package has to pay 60+(50*40) euros
  {
    bank b;
    calendar c;
    person p("Mrs C");
    company mwc;
    mwc.buy_winner_package(p,winner_package_name::executive,p.get_balance(),b,c.get_today());
    const money expected{-2060.0};
    const auto observed = p.get_balance().get_value();
    assert(expected == observed);
  }
  //A person his/her profit is distributed over BankWallet and ShopWallet
  {
    bank b;
    calendar c;
    person p("Mrs D");
    balance sender("test");
    const money profit_euros(40.0);
    b.transfer(
      sender,
      profit_euros,
      ribi::imcw::person::proportion_of_profit_to_bank_wallet,
      p.get_bank_wallet(),
      p.get_shop_wallet(),
      date(2015,6,15)
    );
    const money expected_bank_wallet{30.0};
    const money expected_shop_wallet{10.0};
    const auto observed_bank_wallet = p.get_bank_wallet().get_value();
    const auto observed_shop_wallet = p.get_shop_wallet().get_value();
    assert(expected_bank_wallet == observed_bank_wallet);
    assert(expected_shop_wallet == observed_shop_wallet);
  }
}


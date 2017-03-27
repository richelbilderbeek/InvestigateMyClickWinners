#include "company.h"

#include <boost/test/unit_test.hpp>

#include "bank.h"
#include "calendar.h"
#include "helper.h"
#include "buy_winners_strategy.h"

using namespace ribi::imcw;

BOOST_AUTO_TEST_CASE(imcw_company)
{
  const helper h;
  using boost::gregorian::date;
  //When a company is started, all balances are zero
  {
    bank b;
    calendar c;
    person p("Mr A");
    company mcw;
    BOOST_CHECK(mcw.get_balance_compensation_plan().get_value() == money(0.0));
    BOOST_CHECK(mcw.get_balance_holding ().get_value() == money(0.0));
    BOOST_CHECK(mcw.get_balance_reserves().get_value() == money(0.0));
    //BOOST_CHECK(c.get_customers().empty());
  }
  //When a company is started, all balances are zero
  {
    bank b;
    calendar c;
    person p("Mr B");
    company mcw;
    mcw.buy_winner_package(p,winner_package_name::basic,p.get_balance(),b,date(2015,6,15));
    BOOST_CHECK(mcw.get_balance_compensation_plan().get_value() == money(0.0));
    BOOST_CHECK(mcw.get_balance_holding ().get_value() == money(0.0));
    BOOST_CHECK(mcw.get_balance_reserves().get_value() == money(0.0));
    //BOOST_CHECK(c.get_customers().empty());
  }
  //When a person buys a WinnerPackage he/she is added to the company
  {
    bank b;
    calendar c;
    person p("Mr C");
    company mcw;
    BOOST_CHECK(mcw.get_customers().empty());
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,date(2015,6,15));
    BOOST_CHECK(mcw.get_customers().size() == 1);
  }
  //A person buying a starter winner package results in 100 euros in the undistributed money balance
  {
    bank b;
    calendar c;
    person p("Mr D");
    company mcw;
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,date(2015,6,15));
    const money expected(100.0);
    const auto observed = mcw.get_balance_undistributed().get_value();
    BOOST_CHECK(expected == observed);
  }
  //A person buying an executive winner package has to pay 60+(50*40) euros in the undistributed money balance
  {
    bank b;
    calendar c;
    person p("Mr E");
    company mcw;
    mcw.buy_winner_package(p,winner_package_name::executive,p.get_balance(),b,date(2015,6,15));
    const money expected{2060.0};
    const auto observed = mcw.get_balance_undistributed().get_value();
    BOOST_CHECK(expected == observed);
  }
  //When a person is banned, his/her ClickCard is removed
  {
    bank b;
    calendar c;
    person p("Mr F");
    company mcw;
    mcw.buy_winner_package(
      p,
      winner_package_name::starter,
      p.get_balance(),
      b,
      date(2015,6,15)
    );
    BOOST_CHECK(!p.has_valid_click_card(date(2015,6,15)));
    BOOST_CHECK( p.has_valid_click_card(date(2015,7,1)));
    mcw.ban(p);
    BOOST_CHECK(!p.has_valid_click_card(date(2015,7,1)));
  }
  //When a person is banned, his/her Winners are removed
  {
    bank b;
    calendar c;
    person p("Mr G");
    company mcw;
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,date(2015,6,15));
    BOOST_CHECK(!p.get_winners().empty());
    mcw.ban(p);
    BOOST_CHECK(p.get_winners().empty());
  }
  //When a company distributes profit, it is distributed as expected
  {
    bank b;
    calendar c;
    person p("Mr H");
    company mcw;
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,date(2015,6,15));
    BOOST_CHECK(mcw.get_balance_compensation_plan().get_value() == money(0.0));
    BOOST_CHECK(mcw.get_balance_holding ().get_value() == money(0.0));
    BOOST_CHECK(mcw.get_balance_reserves().get_value() == money(0.0));
    BOOST_CHECK(mcw.get_balance_undistributed().get_value() == money(100.0));
    //the 100 euros that was undistibuted
    //gets distributed, this ends up one the 1 winner of the 1 customer
    mcw.distribute_net_profit(b,c);
    // * compensation_plan: 15%
    // * holding: 10%
    // * reserves: 30%
    // * winners: 45%
    // Thus in this context:
    // * compensation_plan: 15% of 100 euros = 15 euros.
    //     Because no active customers, these 15 euros get transferred to undistributed
    // * holding: 10% of 100 euros = 10 euros
    // * reserves: 30% of 100 euros = 30 euros
    // * winners: 45% of 100 euros = 45 euros.
    //     Because there is one customer with one Winners, this Winner will become 45 euros
    //  * undistributed: 15 euros from compensation plan
    BOOST_CHECK(mcw.get_balance_compensation_plan().get_value() == money(0.0));
    BOOST_CHECK(mcw.get_balance_holding ().get_value() == money(10.0));
    BOOST_CHECK(mcw.get_balance_reserves().get_value() == money(30.0));
    BOOST_CHECK(mcw.get_balance_undistributed().get_value() == money(15.0));
    BOOST_CHECK(p.get_winners().size() == 1);
    BOOST_CHECK(mcw.get_customers().size() == 1);
    BOOST_CHECK(mcw.get_customers()[0].get().get_winners().size() == 1);
    BOOST_CHECK(
        mcw.get_customers()[0].get().get_winners()[0].get_value()
         == money(45.0)
    );
    BOOST_CHECK(p.get_winners()[0].get_value() == money(45.0));
  }
  //Winners end when exceeding 50 euros and are converted to
  //BankWallet and ShopWallet when customer does not automatically
  //buy new Winners
  {
    bank b;
    calendar c;
    person p("Mr I");
    company mcw;
    p.set_winner_buy_strategy(never_buy());
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,date(2015,6,15));
    BOOST_CHECK(mcw.get_balance_compensation_plan().get_value() == money(0.0));
    BOOST_CHECK(mcw.get_balance_holding ().get_value() == money(0.0));
    BOOST_CHECK(mcw.get_balance_reserves().get_value() == money(0.0));
    BOOST_CHECK(mcw.get_balance_undistributed().get_value() == money(100.0));
    //200 euros is distributed (100 already from winners, 100 from test net profit)
    //customer will have one Winner with 90 euro on it,
    //that will break down
    balance net_profit("test net profit",money(100.0));
    mcw.transfer(net_profit,b,c);
    mcw.distribute_net_profit(b,c);
    // * compensation_plan: 15%
    // * holding: 10%
    // * reserves: 30%
    // * winners: 45%
    // Thus in this context:
    // * compensation_plan: 15% of 200 euros = 30 euros.
    //     Because no active customers, these 30 euros get transferred to undistributed
    // * holding: 10% of 200 euros = 20 euros
    // * reserves: 30% of 200 euros = 60 euros
    // * winners: 45% of 200 euros = 90 euros.
    //     Then customer will have one Winner with 90 euro on it, that will expire,
    //     resulting in 2.50 euros in the ShopWallet and 87.50 in the BankWallet
    //  * undistributed: 30 euros from compensation plan
    BOOST_CHECK(net_profit.get_value() == money(0.0));
    BOOST_CHECK(mcw.get_balance_compensation_plan().get_value() == money(0.0));
    BOOST_CHECK(mcw.get_balance_holding ().get_value() == money(20.0));
    BOOST_CHECK(mcw.get_balance_reserves().get_value() == money(60.0));
    BOOST_CHECK(mcw.get_balance_undistributed().get_value() == money(30.0));
    BOOST_CHECK(p.get_winners().size() == 0);
    BOOST_CHECK(p.get_bank_wallet().get_value() == money(87.50));
    BOOST_CHECK(p.get_shop_wallet().get_value() ==  money(2.50));
  }
  //Winners end when exceeding 50 euros and are converted to
  //a new Winner, BankWallet and ShopWallet,
  //as customer does automatically buy new Winners
  {
    bank b;
    calendar c;
    person p("Mr J");
    company mcw;
    date start_date(2015,6,15);
    mcw.buy_winner_package(p,winner_package_name::starter,p.get_balance(),b,start_date);
    BOOST_CHECK(mcw.get_balance_compensation_plan().get_value() == money(0.0));
    BOOST_CHECK(mcw.get_balance_holding ().get_value() == money(0.0));
    BOOST_CHECK(mcw.get_balance_reserves().get_value() == money(0.0));
    //200 euros is distributed (100 already from winners, 100 from test net profit)
    //He/she has one empty winner
    balance net_profit("test net profit",money(100.0));
    mcw.transfer(net_profit,b,c);
    mcw.distribute_net_profit(b,c);
    // * compensation_plan: 15%
    // * holding: 10%
    // * reserves: 30%
    // * winners: 45%
    // Thus in this context:
    // * compensation_plan: 15% of 200 euros = 30 euros.
    //     Because no active customers, these 30 euros get transferred to undistributed
    // * holding: 10% of 200 euros = 20 euros
    // * reserves: 30% of 200 euros = 60 euros
    // * winners: 45% of 200 euros = 90 euros.
    //     Then customer will have one Winner with 90 euro on it, that will expire,
    //     resulting in 2.50 euros in the ShopWallet and 87.50 in the BankWallet.
    //     Because auto_buy is on, the customer will buy two Winners at 40 euros each,
    //     resulting in:
    //     * ShopWallet of 2.50 euros
    //     * BankWallet of 7.50 euros
    //     * 2x Winner of 40 euros
    //  * undistributed:
    //     * 30 euros from compensation plan
    //     * 2x Winner of 40 euros
    BOOST_CHECK(net_profit.get_value() == money(0.0));
    BOOST_CHECK(mcw.get_balance_compensation_plan().get_value() == money(0.0));
    BOOST_CHECK(mcw.get_balance_holding ().get_value() == money(20.0));
    BOOST_CHECK(mcw.get_balance_reserves().get_value() == money(60.0));
    BOOST_CHECK(mcw.get_balance_undistributed().get_value() == money(110.0));
    BOOST_CHECK(p.get_winners().size() == 2);
    BOOST_CHECK(p.get_winners()[0].get_value() == money(0.0));
    BOOST_CHECK(p.get_winners()[1].get_value() == money(0.0));
    BOOST_CHECK(p.get_bank_wallet().get_value() == money(7.50));
    BOOST_CHECK(p.get_shop_wallet().get_value() == money(2.50));
  }
}

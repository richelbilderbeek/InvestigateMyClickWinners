#include "qtinvestigatemyclickwinnersmaindialog.h"

#include "bank.h"
#include "calendar.h"
#include "company.h"
#include "money.h"
#include "person.h"
#include "winner_package.h"
#include "simulation.h"
#include "simulation_parameters.h"

#include <qwt_legend.h>
#include <qwt_point_data.h>
#include <qwt_plot_curve.h>
#include <qwt_legend_data.h>
#include "ui_qtinvestigatemyclickwinnersmaindialog.h"


QtInvestigateMyClickWinnersMainDialog::QtInvestigateMyClickWinnersMainDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::QtInvestigateMyClickWinnersMainDialog),
  m_curve_balance("Personal bank account"),
  m_curve_bank_wallet("BankWallet"),
  m_curve_shop_wallet("ShopWallet"),
  m_curve_winners("Winners")
{
  ui->setupUi(this);
  m_curve_balance.attach(ui->plot);
  m_curve_bank_wallet.attach(ui->plot);
  m_curve_shop_wallet.attach(ui->plot);
  m_curve_winners.attach(ui->plot);
}

QtInvestigateMyClickWinnersMainDialog::~QtInvestigateMyClickWinnersMainDialog()
{
  delete ui;
}

void QtInvestigateMyClickWinnersMainDialog::on_button_run_clicked()
{
  std::vector<double> ts;
  std::vector<double> balance_values;
  std::vector<double> bank_wallets_values;
  std::vector<double> shop_wallets_values;
  std::vector<double> winners_values;

  using ribi::imcw::person;
  using ribi::imcw::simulation;
  using ribi::imcw::simulation_parameters;

  person p("Mister X");
  p.set_auto_buy(true);

  const simulation_parameters parameters(
    p,
    {},
    boost::gregorian::day_clock::local_day(),
    boost::gregorian::day_clock::local_day()
      + boost::gregorian::months(11)
  );
  simulation s(
    parameters
  );

  //Display initial situation
  std::cout << "*****************" << std::endl;
  std::cout << "Initial situation" << '\n';
  std::cout << "*****************" << std::endl;
  std::cout << s.get_company() << std::endl;
  std::cout << "*****************" << std::endl;

  int day = 0;

  while (s.get_calendar().get_today() != parameters.get_end()) {

    s.do_timestep();

    ts.push_back(static_cast<double>(day));
    balance_values.push_back(s.get_focal_person().get_balance().get_value().get_value_euros());
    bank_wallets_values.push_back(s.get_focal_person().get_bank_wallet().get_value().get_value_euros());
    shop_wallets_values.push_back(s.get_focal_person().get_shop_wallet().get_value().get_value_euros());
    winners_values.push_back(
      get_sum_value(s.get_focal_person().get_winners()).get_value_euros()
    );
    ++day;
  }

  #if QWT_VERSION >= 0x060100 || !WIN32
  m_curve_balance.setData(new QwtPointArrayData(&ts[0],&balance_values[0],balance_values.size()));
  m_curve_bank_wallet.setData(new QwtPointArrayData(&ts[0],&bank_wallets_values[0],bank_wallets_values.size()));
  m_curve_shop_wallet.setData(new QwtPointArrayData(&ts[0],&shop_wallets_values[0],shop_wallets_values.size()));
  m_curve_winners.setData(new QwtPointArrayData(&ts[0],&winners_values[0],winners_values.size()));
  #else
  m_curve.setData(&v_x[0],&v_y[0],v_y.size());
  #endif
  ui->plot->replot();
}

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
  m_curve_company_compensation_plan("CompensationPlan"),
  m_curve_company_holding("Holding"),
  m_curve_company_reserves("Reserves"),
  m_curve_company_undistributed("Undistributed"),
  m_curve_focal_person_balance("Personal bank account"),
  m_curve_focal_person_bank_wallet("BankWallet"),
  m_curve_focal_person_shop_wallet("ShopWallet"),
  m_curve_focal_person_winners("Winners")
{
  ui->setupUi(this);
  ui->plot_focal_person->setTitle("Focal person");
  ui->plot_company->setTitle("Company");

  m_curve_company_compensation_plan.setPen(Qt::red);
  m_curve_company_holding.setPen(Qt::green);
  m_curve_company_reserves.setPen(Qt::blue);
  m_curve_company_undistributed.setPen(Qt::black);

  m_curve_focal_person_balance.setPen(Qt::black);
  m_curve_focal_person_bank_wallet.setPen(Qt::red);
  m_curve_focal_person_shop_wallet.setPen(Qt::green);
  m_curve_focal_person_winners.setPen(Qt::blue);

  m_curve_company_compensation_plan.attach(ui->plot_company);
  m_curve_company_holding.attach(ui->plot_company);
  m_curve_company_reserves.attach(ui->plot_company);
  m_curve_company_undistributed.attach(ui->plot_company);

  m_curve_focal_person_balance.attach(ui->plot_focal_person);
  m_curve_focal_person_bank_wallet.attach(ui->plot_focal_person);
  m_curve_focal_person_shop_wallet.attach(ui->plot_focal_person);
  m_curve_focal_person_winners.attach(ui->plot_focal_person);
  {
    QwtLegend * const legend = new QwtLegend;
    legend->setFrameStyle(QFrame::Box | QFrame::Sunken);
    ui->plot_company->insertLegend(legend, QwtPlot::RightLegend);
  }
  {
    QwtLegend * const legend = new QwtLegend;
    legend->setFrameStyle(QFrame::Box | QFrame::Sunken);
    ui->plot_focal_person->insertLegend(legend, QwtPlot::RightLegend);
  }
  on_button_run_clicked();
}

QtInvestigateMyClickWinnersMainDialog::~QtInvestigateMyClickWinnersMainDialog()
{
  delete ui;
}

void QtInvestigateMyClickWinnersMainDialog::on_button_run_clicked()
{
  std::vector<double> ts;

  std::vector<double> company_balance_compensation_plan;
  std::vector<double> company_balance_holding;
  std::vector<double> company_balance_reserves;
  std::vector<double> company_balance_undistributed;

  std::vector<double> focal_person_balance_values;
  std::vector<double> focal_person_bank_wallets_values;
  std::vector<double> focal_person_shop_wallets_values;
  std::vector<double> focal_person_winners_values;

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
  simulation s(parameters);

  int day = 0;

  while (s.get_calendar().get_today() != parameters.get_end()) {

    s.do_timestep();

    //Store everything
    ts.push_back(static_cast<double>(day));
    focal_person_balance_values.push_back(
      s.get_focal_person().get_balance().get_value().get_value_euros()
    );
    focal_person_bank_wallets_values.push_back(
      s.get_focal_person().get_bank_wallet().get_value().get_value_euros()
    );
    focal_person_shop_wallets_values.push_back(
      s.get_focal_person().get_shop_wallet().get_value().get_value_euros()
    );
    focal_person_winners_values.push_back(
      get_sum_value(s.get_focal_person().get_winners()).get_value_euros()
    );
    company_balance_compensation_plan.push_back(
      s.get_company().get_balance_compensation_plan().get_value().get_value_euros()
    );
    company_balance_holding.push_back(
      s.get_company().get_balance_holding().get_value().get_value_euros()
    );
    company_balance_reserves.push_back(
      s.get_company().get_balance_reserves().get_value().get_value_euros()
    );
    company_balance_undistributed.push_back(
      s.get_company().get_balance_undistributed().get_value().get_value_euros()
    );
    ++day;
  }

  #if QWT_VERSION >= 0x060100 || !WIN32
  m_curve_company_compensation_plan.setData(new QwtPointArrayData(&ts[0],&company_balance_compensation_plan[0],company_balance_compensation_plan.size()));
  m_curve_company_holding.setData(new QwtPointArrayData(&ts[0],&company_balance_holding[0],company_balance_holding.size()));
  m_curve_company_reserves.setData(new QwtPointArrayData(&ts[0],&company_balance_reserves[0],company_balance_reserves.size()));
  m_curve_company_undistributed.setData(new QwtPointArrayData(&ts[0],&company_balance_undistributed[0],company_balance_undistributed.size()));
  m_curve_focal_person_balance.setData(new QwtPointArrayData(&ts[0],&focal_person_balance_values[0],focal_person_balance_values.size()));
  m_curve_focal_person_bank_wallet.setData(new QwtPointArrayData(&ts[0],&focal_person_bank_wallets_values[0],focal_person_bank_wallets_values.size()));
  m_curve_focal_person_shop_wallet.setData(new QwtPointArrayData(&ts[0],&focal_person_shop_wallets_values[0],focal_person_shop_wallets_values.size()));
  m_curve_focal_person_winners.setData(new QwtPointArrayData(&ts[0],&focal_person_winners_values[0],focal_person_winners_values.size()));
  #else
  m_curve.setData(&v_x[0],&v_y[0],v_y.size());
  #endif
  ui->plot_focal_person->replot();
}

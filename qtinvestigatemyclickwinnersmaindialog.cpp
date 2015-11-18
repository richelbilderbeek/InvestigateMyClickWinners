#include "qtinvestigatemyclickwinnersmaindialog.h"

#include <sstream>

#include <qwt_legend.h>
#include <qwt_point_data.h>
#include <qwt_plot_curve.h>
#include <qwt_legend_data.h>

#include "buy_winners_strategy.h"
#include "bank.h"
#include "calendar.h"
#include "company.h"
#include "money.h"
#include "person.h"
#include "winner_package.h"
#include "simulation.h"
#include "simulation_parameters.h"

#include "ui_qtinvestigatemyclickwinnersmaindialog.h"


ribi::imcw::QtMainDialog::QtMainDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::QtInvestigateMyClickWinnersMainDialog),
  m_curve_company_compensation_plan("CompensationPlan"),
  m_curve_company_holding("Holding"),
  m_curve_company_reserves("Reserves"),
  m_curve_company_undistributed("Undistributed"),
  m_curve_focal_person_balance("Personal bank account"),
  m_curve_focal_person_bank_wallet("BankWallet"),
  m_curve_focal_person_shop_wallet("ShopWallet"),
  m_curve_focal_person_winners("Winners"),
  m_curve_other_person_balance("Personal bank account"),
  m_curve_other_person_bank_wallet("BankWallet"),
  m_curve_other_person_shop_wallet("ShopWallet"),
  m_curve_other_person_winners("Winners")
{
  ui->setupUi(this);
  ui->plot_focal_person->setTitle("Focal person");
  ui->plot_other_person->setTitle("Other person");
  ui->plot_company->setTitle("Company");

  m_curve_company_compensation_plan.setPen(Qt::red);
  m_curve_company_holding.setPen(Qt::green);
  m_curve_company_reserves.setPen(Qt::blue);
  m_curve_company_undistributed.setPen(Qt::black);

  m_curve_focal_person_balance.setPen(Qt::black);
  m_curve_focal_person_bank_wallet.setPen(Qt::red);
  m_curve_focal_person_shop_wallet.setPen(Qt::green);
  m_curve_focal_person_winners.setPen(Qt::blue);

  m_curve_other_person_balance.setPen(Qt::black);
  m_curve_other_person_bank_wallet.setPen(Qt::red);
  m_curve_other_person_shop_wallet.setPen(Qt::green);
  m_curve_other_person_winners.setPen(Qt::blue);

  m_curve_company_compensation_plan.attach(ui->plot_company);
  m_curve_company_holding.attach(ui->plot_company);
  m_curve_company_reserves.attach(ui->plot_company);
  m_curve_company_undistributed.attach(ui->plot_company);

  m_curve_focal_person_balance.attach(ui->plot_focal_person);
  m_curve_focal_person_bank_wallet.attach(ui->plot_focal_person);
  m_curve_focal_person_shop_wallet.attach(ui->plot_focal_person);
  m_curve_focal_person_winners.attach(ui->plot_focal_person);

  m_curve_other_person_balance.attach(ui->plot_other_person);
  m_curve_other_person_bank_wallet.attach(ui->plot_other_person);
  m_curve_other_person_shop_wallet.attach(ui->plot_other_person);
  m_curve_other_person_winners.attach(ui->plot_other_person);

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
  {
    QwtLegend * const legend = new QwtLegend;
    legend->setFrameStyle(QFrame::Box | QFrame::Sunken);
    ui->plot_other_person->insertLegend(legend, QwtPlot::RightLegend);
  }

  QObject::connect(ui->box_n_membership_years,SIGNAL(valueChanged(int)),this,SLOT(on_button_run_clicked()));
  QObject::connect(ui->box_profit_webshop_euro_per_year,SIGNAL(valueChanged(double)),this,SLOT(on_button_run_clicked()));
  QObject::connect(ui->box_profit_website_euro_per_month,SIGNAL(valueChanged(double)),this,SLOT(on_button_run_clicked()));
  QObject::connect(ui->box_n_other_customers,SIGNAL(valueChanged(int)),this,SLOT(on_button_run_clicked()));
  QObject::connect(ui->box_inspect_customer_index,SIGNAL(valueChanged(int)),this,SLOT(on_button_run_clicked()));
  QObject::connect(ui->box_rng_seed,SIGNAL(valueChanged(int)),this,SLOT(on_button_run_clicked()));

  QObject::connect(ui->box_n_other_customers,SIGNAL(valueChanged(int)),this,SLOT(update_max_inspect_customer_index()));


  update_max_inspect_customer_index();
  on_button_run_clicked();
}

ribi::imcw::QtMainDialog::~QtMainDialog()
{
  delete ui;
}

std::vector<ribi::imcw::person> ribi::imcw::QtMainDialog::create_other_customers() const noexcept
{
  const int sz = ui->box_n_other_customers->value();
  assert(sz >= 0);
  std::vector<person> v;
  for (int i=0; i!=sz; ++i) {
    person p("Other customer #" + std::to_string(i));
    v.push_back(p);
  }
  return v;
}

void ribi::imcw::QtMainDialog::on_button_run_clicked()
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

  std::vector<double> other_person_balance_values;
  std::vector<double> other_person_bank_wallets_values;
  std::vector<double> other_person_shop_wallets_values;
  std::vector<double> other_person_winners_values;

  using ribi::imcw::money;
  using ribi::imcw::person;
  using ribi::imcw::simulation;
  using ribi::imcw::simulation_parameters;
  using boost::gregorian::years;

  const auto today = boost::gregorian::day_clock::local_day();



  const simulation_parameters parameters(
    person(
      "Mister X",
      today + years(ui->box_n_membership_years->value())
    ),
    create_other_customers(),
    today,
    today + years(1 + ui->box_n_membership_years->value()),
    money(ui->box_profit_webshop_euro_per_year->value()),
    money(ui->box_profit_website_euro_per_month->value()),
    ui->box_rng_seed->value()
  );
  assert(parameters.get_profit_webshop_per_year()
    == money(ui->box_profit_webshop_euro_per_year->value())
  );

  simulation s(parameters);

  const int other_person_index = ui->box_inspect_customer_index->value();

  int day = 0;

  while (!s.is_done()) {

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

    if (other_person_index < static_cast<int>(s.get_other_persons().size()))
    {
      assert(other_person_index >= 0);
      other_person_balance_values.push_back(
        s.get_other_persons()[other_person_index].get_balance().get_value().get_value_euros()
      );
      other_person_bank_wallets_values.push_back(
        s.get_other_persons()[other_person_index].get_bank_wallet().get_value().get_value_euros()
      );
      other_person_shop_wallets_values.push_back(
        s.get_other_persons()[other_person_index].get_shop_wallet().get_value().get_value_euros()
      );
      other_person_winners_values.push_back(
        get_sum_value(s.get_other_persons()[other_person_index].get_winners()).get_value_euros()
      );
    }

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
  std::stringstream text;
  text << s.get_bank();
  ui->text_transactions->setPlainText(text.str().c_str());


  assert(s.is_done());
  #if QWT_VERSION >= 0x060100 || !WIN32
  m_curve_company_compensation_plan.setData(new QwtPointArrayData(&ts[0],&company_balance_compensation_plan[0],company_balance_compensation_plan.size()));
  m_curve_company_holding.setData(new QwtPointArrayData(&ts[0],&company_balance_holding[0],company_balance_holding.size()));
  m_curve_company_reserves.setData(new QwtPointArrayData(&ts[0],&company_balance_reserves[0],company_balance_reserves.size()));
  m_curve_company_undistributed.setData(new QwtPointArrayData(&ts[0],&company_balance_undistributed[0],company_balance_undistributed.size()));
  m_curve_focal_person_balance.setData(new QwtPointArrayData(&ts[0],&focal_person_balance_values[0],focal_person_balance_values.size()));
  m_curve_focal_person_bank_wallet.setData(new QwtPointArrayData(&ts[0],&focal_person_bank_wallets_values[0],focal_person_bank_wallets_values.size()));
  m_curve_focal_person_shop_wallet.setData(new QwtPointArrayData(&ts[0],&focal_person_shop_wallets_values[0],focal_person_shop_wallets_values.size()));
  m_curve_focal_person_winners.setData(new QwtPointArrayData(&ts[0],&focal_person_winners_values[0],focal_person_winners_values.size()));
  m_curve_other_person_balance.setData(new QwtPointArrayData(&ts[0],&other_person_balance_values[0],other_person_balance_values.size()));
  m_curve_other_person_bank_wallet.setData(new QwtPointArrayData(&ts[0],&other_person_bank_wallets_values[0],other_person_bank_wallets_values.size()));
  m_curve_other_person_shop_wallet.setData(new QwtPointArrayData(&ts[0],&other_person_shop_wallets_values[0],other_person_shop_wallets_values.size()));
  m_curve_other_person_winners.setData(new QwtPointArrayData(&ts[0],&other_person_winners_values[0],other_person_winners_values.size()));
  #else
  m_curve.setData(&v_x[0],&v_y[0],v_y.size());
  #endif
  ui->plot_company->replot();
  ui->plot_focal_person->replot();
  ui->plot_other_person->replot();
}

void ribi::imcw::QtMainDialog::update_max_inspect_customer_index()
{
  ui->box_inspect_customer_index->setMaximum(
    ui->box_n_other_customers->value() - 1
  );
}

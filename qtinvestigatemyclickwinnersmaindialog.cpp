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
  m_curve("Sine")
{
  ui->setupUi(this);

  std::vector<double> ts;
  std::vector<double> ms;
  for (double x = 0; x < 2.0 * M_PI; x+=(M_PI / 10.0))
  {
    ts.push_back(x);
    ms.push_back(std::sin(x));
  }
  #if QWT_VERSION >= 0x060100 || !WIN32
  m_curve.setData(new QwtPointArrayData(&ts[0],&ms[0],ms.size()));
  #else
  m_curve.setData(&v_x[0],&v_y[0],v_y.size());
  #endif
  m_curve.attach(ui->plot);
  ui->plot->replot();
}

QtInvestigateMyClickWinnersMainDialog::~QtInvestigateMyClickWinnersMainDialog()
{
  delete ui;
}

void QtInvestigateMyClickWinnersMainDialog::on_button_run_clicked()
{
  std::vector<double> ts;
  std::vector<double> ms;

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
    const double m = s.get_focal_person().get_bank_wallet().get_value().get_value_euros();
    ts.push_back(static_cast<double>(day));
    ms.push_back(m);
    ++day;
  }

  #if QWT_VERSION >= 0x060100 || !WIN32
  m_curve.setData(new QwtPointArrayData(&ts[0],&ms[0],ms.size()));
  #else
  m_curve.setData(&v_x[0],&v_y[0],v_y.size());
  #endif
  ui->plot->replot();
}

#include "qtinvestigatemyclickwinnersmaindialog.h"

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

  std::vector<double> xs;
  std::vector<double> ys;
  for (double x = 0; x < 2.0 * M_PI; x+=(M_PI / 10.0))
  {
    xs.push_back(x);
    ys.push_back(std::sin(x));
  }
  #if QWT_VERSION >= 0x060100 || !WIN32
  m_curve.setData(new QwtPointArrayData(&xs[0],&ys[0],ys.size()));
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

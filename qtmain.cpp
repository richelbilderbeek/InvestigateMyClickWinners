#include <QApplication>
#include "qtinvestigatemyclickwinnersmaindialog.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QtInvestigateMyClickWinnersMainDialog w;
  w.show();
  return a.exec();
}

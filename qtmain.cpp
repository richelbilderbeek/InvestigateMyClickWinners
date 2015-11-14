#include <QApplication>
#include "qtinvestigatemyclickwinnersmaindialog.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  ribi::imcw::QtMainDialog w;
  w.show();
  return a.exec();
}

#!/bin/bash
cd ..
git clone https://github.com/richelbilderbeek/RibiClasses
git clone https://github.com/richelbilderbeek/RibiLibraries
cd RibiLibraries; fparser.sh; cd ..
cd InvestigateMyClickWinners
qmake InvestigateMyClickWinnersConsole.pro
make
qmake InvestigateMyClickWinnersDesktop.pro
make

include(../RibiLibraries/ConsoleApplication.pri)
include(../RibiLibraries/Boost.pri)

include(../InvestigateMyClickWinners/InvestigateMyClickWinnersConsole.pri)

SOURCES += main.cpp \
    person.cpp \
    clickcard.cpp \
    winner_package.cpp \
    winner_package_name.cpp \
    winner.cpp \
    company.cpp

HEADERS += \
    person.h \
    clickcard.h \
    winner_package.h \
    winner_package_name.h \
    winner.h \
    company.h

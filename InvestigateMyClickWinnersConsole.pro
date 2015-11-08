include(../RibiLibraries/ConsoleApplication.pri)
include(../RibiLibraries/Boost.pri)

include(../InvestigateMyClickWinners/InvestigateMyClickWinnersConsole.pri)

SOURCES += main.cpp \
    person.cpp \
    clickcard.cpp \
    winner_package.cpp \
    winner_package_name.cpp \
    winner.cpp \
    company.cpp \
    simulation.cpp \
    simulation_parameters.cpp \
    calendar.cpp \
    helper.cpp \
    bank.cpp \
    balance.cpp

HEADERS += \
    person.h \
    clickcard.h \
    winner_package.h \
    winner_package_name.h \
    winner.h \
    company.h \
    simulation.h \
    simulation_parameters.h \
    calendar.h \
    helper.h \
    bank.h \
    balance.h

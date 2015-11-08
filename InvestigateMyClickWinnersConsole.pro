include(../RibiLibraries/ConsoleApplication.pri)
include(../RibiLibraries/Boost.pri)

include(../InvestigateMyClickWinners/InvestigateMyClickWinnersConsole.pri)

SOURCES += main.cpp \
    balance.cpp \
    bank.cpp \
    calendar.cpp \
    clickcard.cpp \
    company.cpp \
    helper.cpp \
    person.cpp \
    simulation.cpp \
    simulation_parameters.cpp \
    winner.cpp \
    winner_package.cpp \
    winner_package_name.cpp

HEADERS += \
    balance.h \
    bank.h \
    calendar.h \
    clickcard.h \
    company.h \
    helper.h \
    person.h \
    simulation.h \
    simulation_parameters.h \
    winner.h \
    winner_package.h \
    winner_package_name.h

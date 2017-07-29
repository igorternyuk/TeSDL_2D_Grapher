TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lSDL2 -lSDL2_ttf -lSDL2_gfx

SOURCES += main.cpp \
    parser.cpp \
    grapher.cpp

QMAKE_CXXFLAGS += -std=c++14

HEADERS += \
    parser.h \
    grapher.h

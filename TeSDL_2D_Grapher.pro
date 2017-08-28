TEMPLATE = app
CONFIG += c++1z
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lSDL2  -lSDL2_ttf -lSDL2_gfx

SOURCES += main.cpp \
    parser.cpp \
    grapher.cpp

HEADERS += \
    parser.h \
    grapher.h

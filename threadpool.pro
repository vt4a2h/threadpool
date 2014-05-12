TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

SOURCES += main.cpp \
    joiner.cc \
    functionwrapper.cc \
    threadpool.cc

LIBS += -lpthread

HEADERS += \
    threadsafequeue.h \
    joiner.h \
    functionwrapper.h \
    threadpool.h


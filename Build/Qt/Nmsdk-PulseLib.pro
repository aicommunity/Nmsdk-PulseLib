#-------------------------------------------------
#
# Project created by QtCreator 2012-11-11T15:17:55
#
#-------------------------------------------------

QT       -= gui

TARGET = Nmsdk-PulseLib.qt
TEMPLATE = lib
CONFIG += staticlib

include($$PWD/../../../../Rdk/Build/Lib/Qt/RdkDefines.pri)

DEFINES += LIBRDK_LIBRARY_EXPORT
DEFINES += RDK_UNICODE_RUN
DEFINES += RDK_QT
INCLUDEPATH += $$PWD/../../../../Rdk/Deploy/Include

VERSION = $$system(hg parents --template '{rev}')
DEFINES += RDK_LIB_CVS_REVISION=$$VERSION

unix {
    target.path = /usr/lib
    INSTALLS += target
    DESTDIR = $$PWD/../../../../Bin/Platform/Linux/Lib.Qt
}

windows {
    DESTDIR = $$PWD/../../../../Bin/Platform/Win/Lib.Qt
}

HEADERS += \
    ../../Core/NAfferentNeuron.h \
    ../../Core/NEyeMuscle.h \
    ../../Core/NMuscle.h \
    ../../Core/NPac.h \
    ../../Core/NPulseChannel.h \
    ../../Core/NPulseChannelCommon.h \
    ../../Core/NPulseHebbSynapse.h \
    ../../Core/NPulseLibrary.h \
    ../../Core/NPulseLTZone.h \
    ../../Core/NPulseLTZoneCommon.h \
    ../../Core/NPulseMembrane.h \
    ../../Core/NPulseMembraneCommon.h \
    ../../Core/NPulseNeuron.h \
    ../../Core/NPulseNeuronCommon.h \
    ../../Core/NPulseSynapse.h \
    ../../Core/NPulseSynapseCommon.h \
    ../../Core/NPulseSynChannel.h \
    ../../Core/NPulseSynapseStdp.h \
    ../../Core/NReceptor.h

SOURCES += \
    ../../Core/NAfferentNeuron.cpp \
    ../../Core/NEyeMuscle.cpp \
    ../../Core/NMuscle.cpp \
    ../../Core/NPac.cpp \
    ../../Core/NPulseChannel.cpp \
    ../../Core/NPulseChannelCommon.cpp \
    ../../Core/NPulseHebbSynapse.cpp \
    ../../Core/NPulseLibrary.cpp \
    ../../Core/NPulseLTZone.cpp \
    ../../Core/NPulseLTZoneCommon.cpp \
    ../../Core/NPulseMembrane.cpp \
    ../../Core/NPulseMembraneCommon.cpp \
    ../../Core/NPulseNeuron.cpp \
    ../../Core/NPulseNeuronCommon.cpp \
    ../../Core/NPulseSynapse.cpp \
    ../../Core/NPulseSynapseCommon.cpp \
    ../../Core/NPulseSynChannel.cpp \
    ../../Core/NPulseSynapseStdp.cpp \
    ../../Core/NReceptor.cpp

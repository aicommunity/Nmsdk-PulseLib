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

INCLUDEPATH += $$PWD/../../../../Rdk/Deploy/Include

VERSION = 0 # $$system(hg parents --template '{rev}')
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
    ../../Core/NAssociationFormer.h \
    ../../Core/NClassifier.h \
    ../../Core/NConditionedReflex.h \
    ../../Core/NConstGenerator.h \
    ../../Core/NConvolutionNet.h \
    ../../Core/NDataset.h \
    ../../Core/NEyeMuscle.h \
    ../../Core/NFileGenerator.h \
    ../../Core/NIntegrateAndFireNeuron.h \
    ../../Core/NLogicalNot.h \
    ../../Core/NMExtrapolator.h \
    ../../Core/NModel.h \
    ../../Core/NMuscle.h \
    ../../Core/NNet.h \
    ../../Core/NNeuron.h \
    ../../Core/NNeuronFreqGroup.h \
    ../../Core/NNeuronFreqGroupLayer.h \
    ../../Core/NNeuronLearner.h \
    ../../Core/NNeuronLife.h \
    ../../Core/NNeuronTrainer.h \
    ../../Core/NNeuronsLayer.h \
    ../../Core/NPCAClassifier.h \
    ../../Core/NPac.h \
    ../../Core/NPainReflexSimple.h \
    ../../Core/NPattern.h \
    ../../Core/NPredictor.h \
    ../../Core/NPulseChannel.h \
    ../../Core/NPulseChannelCable.h \
    ../../Core/NPulseChannelClassic.h \
    ../../Core/NPulseChannelCommon.h \
    ../../Core/NPulseChannelIaF.h \
    ../../Core/NPulseChannelIzhikevich.h \
    ../../Core/NPulseDelay.h \
    ../../Core/NPulseGenerator.h \
    ../../Core/NPulseGeneratorDelay.h \
    ../../Core/NPulseGeneratorMulti.h \
    ../../Core/NPulseHebbLifeSynapse.h \
    ../../Core/NPulseHebbSynapse.h \
    ../../Core/NPulseLTZoneIzhikevich.h \
    ../../Core/NPulseLibrary.h \
    ../../Core/NPulseLTZone.h \
    ../../Core/NPulseLTZoneCommon.h \
    ../../Core/NPulseLifeNeuron.h \
    ../../Core/NPulseMembrane.h \
    ../../Core/NPulseMembraneCommon.h \
    ../../Core/NPulseMembraneIzhikevich.h \
    ../../Core/NPulseNeuron.h \
    ../../Core/NPulseNeuronCommon.h \
    ../../Core/NPulseNeuronIzhikevich.h \
    ../../Core/NPulsePerseptron.h \
    ../../Core/NPulseSynapse.h \
    ../../Core/NPulseSynapseCommon.h \
    ../../Core/NPulseSynChannel.h \
    ../../Core/NPulseSynapseStdp.h \
    ../../Core/NReceiver.h \
    ../../Core/NReceptor.h \
    ../../Core/NSinusGenerator.h \
    ../../Core/NSource.h \
    ../../Core/NSpikeClassifier.h \
    ../../Core/NStatePredictor.h \
    ../../Core/NSum.h \
    ../../Core/NSynapseClassic.h \
    ../../Core/NSynapseClassicSlv.h \
    ../../Core/NSynapseStdp.h \
    ../../Core/NSynapseTrainer.h \
    ../../Core/NSynapseTrainerStdp.h

SOURCES += \
    ../../Core/NAfferentNeuron.cpp \
    ../../Core/NAssociationFormer.cpp \
    ../../Core/NClassifier.cpp \
    ../../Core/NConditionedReflex.cpp \
    ../../Core/NConstGenerator.cpp \
    ../../Core/NConvolutionNet.cpp \
    ../../Core/NDataset.cpp \
    ../../Core/NEyeMuscle.cpp \
    ../../Core/NFileGenerator.cpp \
    ../../Core/NIntegrateAndFireNeuron.cpp \
    ../../Core/NLogicalNot.cpp \
    ../../Core/NMExtrapolator.cpp \
    ../../Core/NModel.cpp \
    ../../Core/NMuscle.cpp \
    ../../Core/NNet.cpp \
    ../../Core/NNeuron.cpp \
    ../../Core/NNeuronFreqGroup.cpp \
    ../../Core/NNeuronFreqGroupLayer.cpp \
    ../../Core/NNeuronLearner.cpp \
    ../../Core/NNeuronLife.cpp \
    ../../Core/NNeuronTrainer.cpp \
    ../../Core/NNeuronsLayer.cpp \
    ../../Core/NPCAClassifier.cpp \
    ../../Core/NPac.cpp \
    ../../Core/NPainReflexSimple.cpp \
    ../../Core/NPattern.cpp \
    ../../Core/NPredictor.cpp \
    ../../Core/NPulseChannel.cpp \
    ../../Core/NPulseChannelCable.cpp \
    ../../Core/NPulseChannelClassic.cpp \
    ../../Core/NPulseChannelCommon.cpp \
    ../../Core/NPulseChannelIaF.cpp \
    ../../Core/NPulseChannelIzhikevich.cpp \
    ../../Core/NPulseDelay.cpp \
    ../../Core/NPulseGenerator.cpp \
    ../../Core/NPulseGeneratorDelay.cpp \
    ../../Core/NPulseGeneratorMulti.cpp \
    ../../Core/NPulseHebbLifeSynapse.cpp \
    ../../Core/NPulseHebbSynapse.cpp \
    ../../Core/NPulseLTZoneIzhikevich.cpp \
    ../../Core/NPulseLibrary.cpp \
    ../../Core/NPulseLTZone.cpp \
    ../../Core/NPulseLTZoneCommon.cpp \
    ../../Core/NPulseLifeNeuron.cpp \
    ../../Core/NPulseMembrane.cpp \
    ../../Core/NPulseMembraneCommon.cpp \
    ../../Core/NPulseMembraneIzhikevich.cpp \
    ../../Core/NPulseNeuron.cpp \
    ../../Core/NPulseNeuronCommon.cpp \
    ../../Core/NPulseNeuronIzhikevich.cpp \
    ../../Core/NPulsePerseptron.cpp \
    ../../Core/NPulseSynapse.cpp \
    ../../Core/NPulseSynapseCommon.cpp \
    ../../Core/NPulseSynChannel.cpp \
    ../../Core/NPulseSynapseStdp.cpp \
    ../../Core/NReceiver.cpp \
    ../../Core/NReceptor.cpp \
    ../../Core/NSinusGenerator.cpp \
    ../../Core/NSource.cpp \
    ../../Core/NSpikeClassifier.cpp \
    ../../Core/NStatePredictor.cpp \
    ../../Core/NSum.cpp \
    ../../Core/NSynapseClassic.cpp \
    ../../Core/NSynapseClassicSlv.cpp \
    ../../Core/NSynapseStdp.cpp \
    ../../Core/NSynapseTrainer.cpp \
    ../../Core/NSynapseTrainerStdp.cpp

cmake_minimum_required(VERSION 3.14)
project(Nmsdk-PulseLib)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include_directories(
    ${CMAKE_SOURCE_DIR}/../../../../Rdk/Deploy/Include
    ${CMAKE_SOURCE_DIR}/../../Core
)

add_definitions(-DRDK_LIB_CVS_REVISION=0)

set(HEADERS
    ${CMAKE_SOURCE_DIR}/../../Core/NAfferentNeuron.h
    ${CMAKE_SOURCE_DIR}/../../Core/NAssociationFormer.h
    ${CMAKE_SOURCE_DIR}/../../Core/NClassifier.h
    ${CMAKE_SOURCE_DIR}/../../Core/NConditionedReflex.h
    ${CMAKE_SOURCE_DIR}/../../Core/NConstGenerator.h
    ${CMAKE_SOURCE_DIR}/../../Core/NConvolutionNet.h
    ${CMAKE_SOURCE_DIR}/../../Core/NDataset.h
    ${CMAKE_SOURCE_DIR}/../../Core/NEyeMuscle.h
    ${CMAKE_SOURCE_DIR}/../../Core/NFileGenerator.h
    ${CMAKE_SOURCE_DIR}/../../Core/NIntegrateAndFireNeuron.h
    ${CMAKE_SOURCE_DIR}/../../Core/NLogicalNot.h
    ${CMAKE_SOURCE_DIR}/../../Core/NMExtrapolator.h
    ${CMAKE_SOURCE_DIR}/../../Core/NModel.h
    ${CMAKE_SOURCE_DIR}/../../Core/NMuscle.h
    ${CMAKE_SOURCE_DIR}/../../Core/NNet.h
    ${CMAKE_SOURCE_DIR}/../../Core/NNeuron.h
    ${CMAKE_SOURCE_DIR}/../../Core/NNeuronFreqGroup.h
    ${CMAKE_SOURCE_DIR}/../../Core/NNeuronFreqGroupLayer.h
    ${CMAKE_SOURCE_DIR}/../../Core/NNeuronLearner.h
    ${CMAKE_SOURCE_DIR}/../../Core/NNeuronLife.h
    ${CMAKE_SOURCE_DIR}/../../Core/NNeuronTrainer.h
    ${CMAKE_SOURCE_DIR}/../../Core/NNeuronsLayer.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPCAClassifier.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPac.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPainReflexSimple.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPattern.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPredictor.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseChannel.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseChannelCable.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseChannelClassic.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseChannelCommon.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseChannelIaF.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseChannelIzhikevich.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseDelay.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseGenerator.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseGeneratorDelay.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseGeneratorMulti.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseHebbLifeSynapse.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseHebbSynapse.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseLTZoneIzhikevich.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseLibrary.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseLTZone.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseLTZoneCommon.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseLifeNeuron.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseMembrane.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseMembraneCommon.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseMembraneIzhikevich.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseNeuron.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseNeuronCommon.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseNeuronIzhikevich.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulsePerseptron.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseSynapse.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseSynapseCommon.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseSynChannel.h
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseSynapseStdp.h
    ${CMAKE_SOURCE_DIR}/../../Core/NReceiver.h
    ${CMAKE_SOURCE_DIR}/../../Core/NReceptor.h
    ${CMAKE_SOURCE_DIR}/../../Core/NSinusGenerator.h
    ${CMAKE_SOURCE_DIR}/../../Core/NSource.h
    ${CMAKE_SOURCE_DIR}/../../Core/NSpikeClassifier.h
    ${CMAKE_SOURCE_DIR}/../../Core/NStatePredictor.h
    ${CMAKE_SOURCE_DIR}/../../Core/NSum.h
    ${CMAKE_SOURCE_DIR}/../../Core/NSynapseClassic.h
    ${CMAKE_SOURCE_DIR}/../../Core/NSynapseClassicSlv.h
    ${CMAKE_SOURCE_DIR}/../../Core/NSynapseStdp.h
    ${CMAKE_SOURCE_DIR}/../../Core/NSynapseTrainer.h
    ${CMAKE_SOURCE_DIR}/../../Core/NSynapseTrainerStdp.h
)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/../../Core/NAfferentNeuron.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NAssociationFormer.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NClassifier.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NConditionedReflex.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NConstGenerator.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NConvolutionNet.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NDataset.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NEyeMuscle.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NFileGenerator.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NIntegrateAndFireNeuron.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NLogicalNot.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NMExtrapolator.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NModel.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NMuscle.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NNet.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NNeuron.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NNeuronFreqGroup.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NNeuronFreqGroupLayer.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NNeuronLearner.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NNeuronLife.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NNeuronTrainer.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NNeuronsLayer.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPCAClassifier.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPac.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPainReflexSimple.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPattern.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPredictor.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseChannel.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseChannelCable.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseChannelClassic.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseChannelCommon.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseChannelIaF.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseChannelIzhikevich.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseDelay.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseGenerator.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseGeneratorDelay.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseGeneratorMulti.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseHebbLifeSynapse.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseHebbSynapse.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseLTZoneIzhikevich.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseLibrary.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseLTZone.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseLTZoneCommon.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseLifeNeuron.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseMembrane.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseMembraneCommon.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseMembraneIzhikevich.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseNeuron.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseNeuronCommon.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseNeuronIzhikevich.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulsePerseptron.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseSynapse.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseSynapseCommon.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseSynChannel.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NPulseSynapseStdp.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NReceiver.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NReceptor.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NSinusGenerator.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NSource.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NSpikeClassifier.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NStatePredictor.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NSum.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NSynapseClassic.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NSynapseClassicSlv.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NSynapseStdp.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NSynapseTrainer.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/NSynapseTrainerStdp.cpp
)

add_library(Nmsdk-PulseLib STATIC)

if(UNIX)
    set_target_properties(Nmsdk-PulseLib PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../../../Bin/Platform/Linux/Lib.Qt
    )
elseif(WIN32)
    set_target_properties(Nmsdk-PulseLib PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../../../Bin/Platform/Win/Lib.Qt
    )
endif()

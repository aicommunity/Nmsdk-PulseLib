#ifndef NCLASSIFIERDYNAMICSRECORDER_H
#define NCLASSIFIERDYNAMICSRECORDER_H

#include "../../../Rdk/Deploy/Include/rdk.h"
#include <fstream>
#include <string>
#include <vector>

namespace NMSDK {

using namespace RDK;

/// Passive CSV recorder for signed classifier and neuron signals.
class RDK_LIB_TYPE NClassifierDynamicsRecorder : public UNet
{
public:
 UProperty<std::vector<MDMatrix<double> >, NClassifierDynamicsRecorder, ptInput | ptPubState> Signals;
 UProperty<std::string, NClassifierDynamicsRecorder, ptPubParameter> SignalNamesCsv;
 UProperty<std::string, NClassifierDynamicsRecorder, ptPubParameter> SavePath;
 UProperty<std::string, NClassifierDynamicsRecorder, ptPubParameter> FileName;
 UProperty<double, NClassifierDynamicsRecorder, ptPubParameter> SampleInterval;
 UProperty<double, NClassifierDynamicsRecorder, ptPubParameter> FlushInterval;
 UProperty<bool, NClassifierDynamicsRecorder, ptPubParameter> AppendMode;
 UProperty<bool, NClassifierDynamicsRecorder, ptPubParameter> Enable;
 UProperty<std::string, NClassifierDynamicsRecorder, ptOutput | ptPubState> LastError;
 UProperty<int, NClassifierDynamicsRecorder, ptOutput | ptPubState> SamplesWritten;

 NClassifierDynamicsRecorder(void);
 virtual ~NClassifierDynamicsRecorder(void);
 virtual NClassifierDynamicsRecorder* New(void);
 static UComponent* NewStatic(void);

protected:
 virtual bool ADefault(void);
 virtual bool ABuild(void);
 virtual bool AReset(void);
 virtual bool ACalculate(void);

 bool EnsureCsvReady(void);
 void CloseCsv(void);
 std::string FullCsvPath(void) const;
 std::vector<std::string> SignalNames(void) const;

 std::ofstream csv_;
 std::string csv_full_path_;
 double last_sample_time_;
 double last_flush_time_;
};

}

#endif

#ifndef NDATASET_H
#define NDATASET_H

#include "NDatasetBase.h"

namespace NMSDK {

using namespace RDK;

/// File-backed dataset: loads feature/class matrices from disk and drives pulse generators
class RDK_LIB_TYPE NDataset: public NDatasetBase
{
public:
/// Request dataset reload from file (cleared after successful load)
UProperty<bool, NDataset, ptPubParameter> ReloadDataset;

/// Dataset file path (absolute, or relative to Config/Work per flags)
UProperty<std::string, NDataset, ptPubParameter> FileName;

/// Interpret FileName relative to the project config/data directory
UProperty<bool, NDataset, ptPubParameter> UseRelativePathFromConfig;

/// Interpret FileName relative to the application work directory
UProperty<bool, NDataset, ptPubParameter> UseRelativePathFromWorkDir;

public:
NDataset(void);
virtual ~NDataset(void);

bool SetFileName(const std::string &value);
bool SetUseRelativePathFromConfig(const bool &value);
bool SetUseRelativePathFromWorkDir(const bool &value);

virtual NDataset* New(void);
static UComponent* NewStatic(void);

protected:
/// Result of attempting to read FileName into MatrixData/MatrixClasses
enum class FileLoadResult
{
    Loaded,      ///< File parsed into matrices
    Unavailable, ///< Empty path or file missing — idle empty matrices OK
    Invalid      ///< File opened but content/schema is wrong
};

/// Load matrices from FileName; Unavailable clears matrices and keeps ReloadDataset
FileLoadResult TreatDataFromFile(void);

/// Resolve FileName using absolute / Config / Work rules
std::string CalcActualSourceFilePath(const std::string &file_name);

virtual bool PrepareDataset(void);
virtual bool ADefault(void);
virtual bool ACalculate(void);
};

}
#endif

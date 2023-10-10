#ifndef NDATASET_H
#define NDATASET_H

#include "NPulseGenerator.h"
#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

// Класс, отвечающий за датасет
class RDK_LIB_TYPE NDataset: public UNet
{
/// Имя класса, создающего генератор импульсов
ULProperty<std::string, NDataset, ptPubParameter> PulseGeneratorClassName;

///Число генераторов
ULProperty<int,NDataset, ptPubParameter> NumGenerators;

///Число признаков (из файла)
ULProperty<int,NDataset, ptPubParameter> NumFeatures;

///Число примеров (из файла)
ULProperty<int,NDataset, ptPubParameter> NumSamples;

///Матрица считанных фьюч
ULProperty<MDMatrix<double>,NDataset, ptPubParameter> MatrixData;

///Матрица считанных классов
ULProperty<MDMatrix<int>,NDataset, ptPubParameter> MatrixClasses;

///Флаг повтора процедуры загрузки данных
ULProperty<bool,NDataset, ptPubParameter> ReloadDataset;

///Матрица временных сдвигов запусков генераторов
ULProperty<MDMatrix<double>,NDataset, ptPubState> MatrixDelay;

///Номер строки в датасете
ULProperty<int,NDataset, ptPubParameter> Iteration;

///Коэффициент во временной кодировке
ULProperty<float,NDataset, ptPubParameter> Tay;

/// Время задержки начала обучения относительно старта системы (сек)
ULProperty<double, NDataset, ptPubParameter> Delay;

///Параметр активации, режим выдачи данных: если =0, то генераторы выключены
///если =1, то показывают время выдачи сигнала
///если =2, то работают бесконечное время
ULProperty<int, NDataset, ptPubState> StateGeneration;

///Время генерации, в секундах
ULProperty<double, NDataset, ptPubState> TimeGeneration;

///Время работы
ULProperty<double, NDataset, ptPubState> OperatingTime;

///Параметр изменения работы генераторов
ULProperty<bool, NDataset, ptPubState> ResetDelay;

/// Частота генераторов (Гц)
ULProperty<double, NDataset, ptPubParameter> SpikesFrequency;

/// Число классифицируемых классов
ULProperty<int,NDataset, ptPubState> NumClasses;

/// Генераторы
vector<NPulseGeneratorTransit*> Generators;

///Файл с датасетом
ULProperty<string,NDataset, ptPubState> FileName;

protected:

 // <-------

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NDataset(void);
virtual ~NDataset(void);
// --------------------------

// --------------------------
// Методы упраления параметрами
// --------------------------

/// Имя класса, создающего генератор импульсов
bool SetPulseGeneratorClassName(const std::string &value);

///Число генераторов
bool SetNumGenerators(const int &value);

///Число признаков (из файла)
bool SetNumFeatures(const int &value);

///Матрица считанных фьюч
bool SetMatrixData(const MDMatrix<double> &value);

///Матрица считанных классов
bool SetMatrixClasses(const MDMatrix<int> &value);

/// Частота генераторов (Гц)
bool SetSpikesFrequency(const double &value);

/// Время задержки начала обучения относительно старта системы (сек)
bool SetDelay(const double &value);

/// Выполняет завершающие пользовательские действия
/// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был успешно добавлен в список компонент
virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer = 0);

/// Выполняет предварительные пользовательские действия
/// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp существует в списке компонент
virtual bool ADelComponent(UEPtr<UContainer> comp);

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NDataset* New(void);
static UComponent* NewStatic(void);
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Функция для работы с файлами.
// Осуществляет чтение входных данных из файла,
// Обработку результатов
bool TreatDataFromFile(void);

// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
virtual bool ABuild(void);

// Сброс процесса счета.
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);
// --------------------------
};

}
#endif

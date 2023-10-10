#ifndef NMSDK_NSYNAPSECLASSICSLV_H
#define NMSDK_NSYNAPSECLASSICSLV_H

#include "NSynapseClassic.h"

namespace NMSDK {

/// Структура описывающая элемент данных для модели синапса
struct NSynapseSpikeData
{
/// Временная метка модельного времени в которое был спайк (сек)
double SpikeTimeStamp;

/// Вес синапса в момент спайка
double Weight;

NSynapseSpikeData(void);
NSynapseSpikeData(const NSynapseSpikeData& value);
};

class NSynapseClassicSlv: public NSynapseClassic
{
public: // Параметры
/// Длина кольцевого буфера для хранения спайков
ULProperty<int, NSynapseClassicSlv, ptPubParameter> SpikeBufferSize;

protected: // Временные перемменные
/// Кольцевой буфер для хранения моментов времени прихода спайков
std::vector<NSynapseSpikeData> SpikeBuffer;

/// Индекс первого и последнего эелмента в кольцевом буфере
size_t FirstSpikeBufferIndex, LastSpikeBufferIndex;

/// Предыдущее значение входного сигнала
MDMatrix<double> PreviousInput;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSynapseClassicSlv(void);
virtual ~NSynapseClassicSlv(void);
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
bool SetSpikeBufferSize(const int &value);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NSynapseClassicSlv* New(void);
// --------------------------        

// --------------------------
// Скрытые методы управления счетом 
// --------------------------
protected:
/// расчет тока синапса во временной точке
/// time_value в секундах от прихода импульса
double CalcCurrent(double tau_s, double current, double time_value, double weight);

// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
virtual bool ABuild(void);

// Сброс процесса счета без потери настроек
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate2(void);
// --------------------------
};


}

#endif




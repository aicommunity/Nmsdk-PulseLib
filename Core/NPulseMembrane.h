// ===========================================================
// Version:        3.0.2
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2002.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_MEMBRANE_H
#define NPULSE_MEMBRANE_H

#include "NPulseMembraneCommon.h"
#include "NPulseChannel.h"


namespace NMSDK {

class NPulseNeuron;

class RDK_LIB_TYPE NPulseMembrane: public NPulseMembraneCommon
{
public: //  
/// Коэффициент обратной связи
UProperty<double,NPulseMembrane, ptPubParameter> FeedbackGain;

/// Наличие механизма сброса
UProperty<bool,NPulseMembrane, ptPubParameter> ResetAvailable;

/// Имя класса синапса
UProperty<std::string, NPulseMembrane, ptPubParameter> SynapseClassName;

/// Имя класса ионного канала
UProperty<std::string, NPulseMembrane, ptPubParameter> ExcChannelClassName;

/// Имя класса ионного канала
UProperty<std::string, NPulseMembrane, ptPubParameter> InhChannelClassName;

/// Число возбуждающих синапсов
UProperty<int, NPulseMembrane, ptPubParameter> NumExcitatorySynapses;

/// Число тормозных синапсов
UProperty<int, NPulseMembrane, ptPubParameter> NumInhibitorySynapses;

public: //   
/// Сигнал обратной связи от низкопороговой зоны
UProperty<MDMatrix<double>, NPulseMembrane, ptInput | ptPubState> InputFeedbackSignal;


protected: //  
// Ионные механизмы деполяризации
vector<NPulseChannelCommon*> ExcitatoryChannels;

// Ионные механизмы гиперполяризации
vector<NPulseChannelCommon*> InhibitoryChannels;

// Возбуждающие синапсы
vector<NPulseSynapseCommon*> ExcitatorySynapses;

// Тормозные синапсы
vector<NPulseSynapseCommon*> InhibitorySynapses;


public: // 
// --------------------------
// --------------------------
// Конструкторы и деструкторы
// --------------------------
// --------------------------
NPulseMembrane(void);
virtual ~NPulseMembrane(void);
// --------------------------

// --------------------------
// --------------------------
// Методы управления временными перменными
// --------------------------
// Ионные механизмы деполяризации
// --------------------------
size_t GetNumPosChannels(void) const;
NPulseChannelCommon* GetPosChannel(size_t i);

// Ионные механизмы гиперполяризации
size_t GetNumNegChannels(void) const;
NPulseChannelCommon* GetNegChannel(size_t i);

//  
size_t GetNumExcitatorySynapses(void) const;
NPulseSynapseCommon* GetExcitatorySynapses(size_t i);

//  
size_t GetNumInhibitorySynapses(void) const;
NPulseSynapseCommon* GetInhibitorySynapses(size_t i);

virtual bool UpdateChannelData(UEPtr<NPulseChannelCommon> channel, UEPtr<UIPointer> pointer=0);

virtual bool UpdateSynapseData(UEPtr<NPulseSynapseCommon> synapse, UEPtr<UIPointer> pointer=0);
// --------------------------

// --------------------------
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
/// Наличие механизма сброса
// --------------------------
bool SetResetAvailable(const bool &value);

// Коэффициент обратной связи
bool SetFeedbackGain(const double &value);

bool SetSynapseClassName(const std::string &value);
bool SetExcChannelClassName(const std::string &value);
bool SetInhChannelClassName(const std::string &value);
bool SetNumExcitatorySynapses(const int &value);
bool SetNumInhibitorySynapses(const int &value);
// --------------------------

// --------------------------
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
// --------------------------
virtual NPulseMembrane* New(void);

protected:
//    
//       
//      comp 
//     
virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

//    
//       
//      comp
//    
virtual bool ADelComponent(UEPtr<UContainer> comp);
// --------------------------

// --------------------------
// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
// --------------------------
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;
// --------------------------

// --------------------------
// --------------------------
// Скрытые методы управления счетом
// --------------------------
// --------------------------
protected:
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
virtual bool ACalculate2(void);

//      
void UpdateComputationOrder(void);
// --------------------------
};

}
#endif


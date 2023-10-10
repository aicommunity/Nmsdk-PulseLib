#ifndef NRESULT_DECODER_H
#define NRESULT_DECODER_H

#include "NNet.h"


namespace NMSDK {

class RDK_LIB_TYPE NResultDecoder: public RDK::UNet
{

//Статистические выходы могут быть

public: // Входы и выходы
/// Входны нейронов персептрона
UPropertyInputCData<MDMatrix<double>, NResultDecoder, ptInput | ptPubState> PerseptronInput;

/// Результат
UPropertyOutputData<MDMatrix<double>, NResultDecoder, ptOutput | ptPubState> Result;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NResultDecoder(void);
virtual ~NResultDecoder(void);

public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NResultDecoder* New(void);

protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
virtual bool ABuild(void);

// Сброс процесса счета без потери настроек
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);


};

}

#endif

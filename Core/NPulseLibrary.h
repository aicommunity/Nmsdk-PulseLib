/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2009.
E-mail:            alexab@ailab.ru
url:            http://ailab.ru
Version:        1.0.0

This file - part of the project: NMSDK

File License:        BSD License
Project License:    GNU GPLv3 License
See file license.txt for more information
*********************************************************** */

#ifndef NPulseLibraryH
#define NPulseLibraryH

#include "NAfferentNeuron.h"
#include "NEyeMuscle.h"
#include "NMuscle.h"
#include "NPulseChannel.h"
#include "NPulseLTZone.h"
#include "NPulseMembrane.h"
#include "NPulseNeuron.h"
#include "NPulseSynapse.h"
#include "NPulseHebbSynapse.h"
#include "NPulseSynChannel.h"
#include "NPac.h"
#include "NReceptor.h"



namespace NMSDK {

class RDK_LIB_TYPE NPulseLibrary: public ULibrary
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseLibrary(void);
// --------------------------

// --------------------------
// Методы заполенения бибилиотеки
// --------------------------
// Заполняет массив ClassSamples готовыми экземплярами образцов и их именами.
// Не требуется предварительная очистка массива и уборка памяти.
virtual void CreateClassSamples(UStorage *storage);
// --------------------------
};                  

extern RDK_LIB_TYPE NPulseLibrary PulseLibrary;

}


#endif

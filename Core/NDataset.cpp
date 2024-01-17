#ifndef NDATASET_CPP
#define NDATASET_CPP

#include "NDataset.h"

// Класс, отвечающий за датасет
namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NDataset::NDataset(void)
: PulseGeneratorClassName("PulseGeneratorClassName",this,&NDataset::SetPulseGeneratorClassName),
  NumGenerators("NumGenerators",this,&NDataset::SetNumGenerators),
  NumFeatures("NumFeatures",this,&NDataset::SetNumFeatures),
  NumSamples("NumSamples",this),
  MatrixData("MatrixData",this,&NDataset::SetMatrixData),
  MatrixClasses("MatrixClasses",this,&NDataset::SetMatrixClasses),
  ReloadDataset("ReloadDataset",this),
  MatrixDelay("MatrixDelay",this),
  Iteration("Iteration",this),
  Tay("Tay",this),
  Delay("Delay",this,&NDataset::SetDelay),
  StateGeneration("StateGeneration",this),
  TimeGeneration("TimeGeneration",this),
  OperatingTime("OperatingTime",this),
  ResetDelay("ResetDelay",this),
  SpikesFrequency("SpikesFrequency",this,&NDataset::SetSpikesFrequency),
  NumClasses("NumClasses",this),
  FileName("FileName",this)
{
  Generators.clear();
}

NDataset::~NDataset(void)
{
}

// --------------------------
// Методы упраления параметрами
// --------------------------

///Число генераторов
bool NDataset::SetNumGenerators(const int &value)
{
    Ready=false;
    return true;
}

///Число признаков (из файла)
bool NDataset::SetNumFeatures(const int &value)
{
    Ready=false;
    return true;
}

///Матрица считанных фьюч
bool NDataset::SetMatrixData(const MDMatrix<double> &value)
{
    Ready=false;
    return true;
}

///Матрица считанных классов
bool NDataset::SetMatrixClasses(const MDMatrix<int> &value)
{
    Ready=false;
    return true;
}

///Матрица временных сдвигов запусков генераторов
MDMatrix<double> SetMatrixDelay(int NumSamples, int NumFeatures, const MDMatrix<double> &MatrixData, double Tay)
{
    MDMatrix<double> matrix_delay;
    matrix_delay.Resize(NumSamples,NumFeatures);
    vector<double> max_features(NumFeatures);
    vector<double> min_features(NumFeatures);
    for(int i=0;i<NumFeatures;i++)
    {
        max_features[i]=MatrixData(0,i);
        min_features[i]=MatrixData(0,i);
        for(int j=1;j<NumSamples;j++)
        {
            if(MatrixData(j,i)>max_features[i])
                max_features[i]=MatrixData(j,i);
            if(MatrixData(j,i)<min_features[i])
                min_features[i]=MatrixData(j,i);
        }
    }
    for(int i=0;i<NumSamples;i++)
    {
        for(int j=0;j<NumFeatures;j++)
        {
            matrix_delay(i,j)=(MatrixData(i,j)-min_features[j])/(max_features[j]-min_features[j])*Tay;
        }
    }
    return matrix_delay;
}

/// Число классов
int SetNumClasses(const MDMatrix<int> &matrix_classes)//можно объявить в h
{
    vector<double> uniq;
    for(int i=0;i<matrix_classes.GetCols();i++)
    {
        bool flagInsert=true;
        for (int j = 0; j < int(uniq.size()); j++)
                    if (matrix_classes(0,i)==uniq[j])
                        flagInsert = false;
                if (flagInsert)
                    uniq.push_back(matrix_classes(0,i));
    }
    return int(uniq.size());
}

/// Имя класса, создающего генератор импульсов
bool NDataset::SetPulseGeneratorClassName(const std::string &value)
{
	Ready=false;
	return true;
}

/// Частота генераторов (Гц)
bool NDataset::SetSpikesFrequency(const double &value)
{
    for(size_t i = 0; i < Generators.size(); i++)
    {
     Generators[i]->Frequency = value;
     Generators[i]->Reset();
    }

	return true;
}


/// Время задержки начала обучения относительно старта системы (сек)
bool NDataset::SetDelay(const double &value)
{
 for(int i = 0; i < int(Generators.size()); i++)
 {
  Generators[i]->Delay = value + MatrixDelay(Iteration,i);
  Generators[i]->Reset();
 }
 return true;
}

// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NDataset* NDataset::New(void)
{
    return new NDataset;
}

UComponent* NDataset::NewStatic(void)
{
    return new NDataset;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool NDataset::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
    UEPtr<NPulseGeneratorTransit> generator=dynamic_pointer_cast<NPulseGeneratorTransit>(comp);
    if(generator)
    {
      if(find(Generators.begin(),Generators.end(),generator) == Generators.end())
       Generators.push_back(generator);
    }
    return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool NDataset::ADelComponent(UEPtr<UContainer> comp)
{
    UEPtr<NPulseGeneratorTransit> generator=dynamic_pointer_cast<NPulseGeneratorTransit>(comp);
    if(generator)
    {
     vector<NPulseGeneratorTransit*>::iterator I;
     I=find(Generators.begin(),Generators.end(),generator.Get());
     if(I != Generators.end())
      Generators.erase(I);
    }

    return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Сброс процесса счета.
bool NDataset::AReset(void)
{
    ResetDelay=true;
	return true;
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool NDataset::ADefault(void)
{
    Tay=float(0.01);
    Iteration=0;
    ReloadDataset=false;
    NumClasses=0;
    StateGeneration=2;
    SpikesFrequency=5;
    Delay=0;
    OperatingTime=0;
    ResetDelay=true;
    FileName="input_data.txt";
    TimeGeneration=5;
    PulseGeneratorClassName = "NPulseGeneratorTransit";
    Generators.clear();
	return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NDataset::ABuild(void)
{
    TreatDataFromFile();
    int old_ex_generators=int(Generators.size());
    for(int i=NumGenerators;i<old_ex_generators;i++)
    {
     UEPtr<UContainer> gen = GetComponentL(std::string("Generator")+sntoa(i+1), true);
     if(gen)
      DelComponent(gen, true);
      //GetStorage()->ReturnObject(gen);
    }

    for(int i=0;i<NumGenerators;i++)
    {
     UEPtr<NPulseGeneratorTransit> generator=AddMissingComponent<NPulseGeneratorTransit>(std::string("Generator")+sntoa(i+1), PulseGeneratorClassName);
     generator->SetCoord(MVector<double,3>(5+i*6,1.7,0));
    }
  return true;
}


// Функция для работы с файлами.
// Осуществляет чтение входных данных из файла,
// Обработку результатов и запись результатов в файл
bool NDataset::TreatDataFromFile(void)
{
    Ready=false;
    // Файл с входными данными
    ifstream fileDataset;
    NumFeatures=0;
    NumSamples=0;
    //fileDataset.open(Environment->GetCurrentDataDir()+FileName.GetData());
     fileDataset.open("C:\\Users\\kirik\\Desktop\\input_data.txt");
    if (!fileDataset)
    {
        return false;
    }
    string line;
    char delimiter=';';
    getline(fileDataset, line);
    for(int i=0;i<line.length();i++)
    {
        if(line[i]==';') NumFeatures++;
    }
    while(getline(fileDataset,line)) NumSamples++;
    MatrixData.Resize(NumSamples,NumFeatures);
    MatrixClasses.Resize(1,NumSamples);
    fileDataset.close();
    //fileDataset.open(Environment->GetCurrentDataDir()+"input_data.txt");
     fileDataset.open("C:\\Users\\kirik\\Desktop\\input_data.txt");
    if (!fileDataset)
    {
        return false;
    }
    getline(fileDataset, line);
    int row=0;
    while (getline(fileDataset,line))
    {
        int col=0;
        stringstream stream(line);
        string number;
        getline(stream,number,delimiter);
        MatrixClasses(0,row)=atoi(number);
        string count;
        while (getline(stream,count,delimiter))
        {
            MatrixData(row,col)=atof(count.c_str());
            col++;
        }
        row++;
    }
    fileDataset.close();
    MatrixDelay = SetMatrixDelay(NumSamples, NumFeatures, MatrixData, Tay);
    NumClasses = SetNumClasses(MatrixClasses);
    NumGenerators = NumFeatures;
    return true;
}


// Выполняет расчет этого объекта
bool NDataset::ACalculate(void)
{
    if(StateGeneration==0)
    {
        for(int i = 0; i < int(Generators.size()); i++)
        {
         Generators[i]->Frequency = 0;
         Generators[i]->Reset();
        }
    }
    else if(StateGeneration==2)
    {
        if(ResetDelay)
        {
            SetDelay(Delay);
            SetSpikesFrequency(SpikesFrequency);
            ResetDelay=false;
        }
    }
    else if(StateGeneration==1)
    {
        if(ResetDelay)
        {
            SetDelay(Delay);
            SetSpikesFrequency(SpikesFrequency);
            OperatingTime = Environment->GetTime().GetDoubleTime();
            ResetDelay=false;
        }
        if(Environment->GetTime().GetDoubleTime()-OperatingTime>TimeGeneration)
        {
            for(int i = 0; i < int(Generators.size()); i++)
            {
             Generators[i]->Frequency = 0;
             Generators[i]->Reset();
            }
            //ResetDelay=true;
        }
    }
    else return false;
    return true; //В ресет сбросить всё
}

// --------------------------


}
#endif

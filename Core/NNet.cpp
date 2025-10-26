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

#ifndef NNET_CPP
#define NNET_CPP

#include "NNet.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NNet::NNet(void)
: LinkDistance("LinkDistance",this),
  DefaultComponentClassId("DefaultComponentClassId",this),
  NetType("NetType",this),
  NumLayers("NumLayers",this),
  LayerWidth("LayerWidth",this),
  LayerHeight("LayerHeight",this),
  ReceptorFieldWidth("ReceptorFieldWidth",this),
  ReceptorFieldHeight("ReceptorFieldHeight",this),
  LinksOrganizationMode("LinkOrganizationMode",this),

  NetStructure("NetStructure",this)

{
}

NNet::~NNet(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NNet* NNet::New(void)
{
 return new NNet;
}
// --------------------------

// --------------------------
// ������ ���������� ���������� ����
// --------------------------
// ������� ����� ��������� ���������� �� ��������� � ��������� ��� � ����
std::shared_ptr<RDK::UContainer> NNet::InsertComponent(void)
{
 auto storage = Storage.lock();
 if(!storage || DefaultComponentClassId == ForbiddenId)
  return 0;

 std::shared_ptr<RDK::UContainer> comp=std::static_pointer_cast<RDK::UContainer>(storage->TakeObject(DefaultComponentClassId));
 if(!comp)
  return 0;

 if(AddComponent(comp) == ForbiddenId)
 {
  comp->Free();
//  Storage->ReturnObject(comp);
  return 0;
 }

 return comp;
}
// --------------------------



// --------------------------
// ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NNet::Default(void)
{
 if(!UNet::Default())
  return false;

 DefaultComponentClassId=ForbiddenId;
 NetType=0;
 NumLayers=1;
 LayerWidth=1;
 LayerHeight=1;
 ReceptorFieldWidth=1;
 ReceptorFieldHeight=1;
 LinksOrganizationMode=0;
 LinkDistance=1;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NNet::Build(void)
{
 if(!BuildNetStructure())
  return false;

 if(!BuildLinksStructure())
  return false;

 return UNet::Build();
}

// ����� �������� ����� ��� ������ ��������
bool NNet::Reset(void)
{
 return UNet::Reset();
}
// --------------------------


// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������� ��������� ����
bool NNet::BuildNetStructure(void)
{
/* switch(NetType.v)
 {
 case 0:
 break;

 case 1:
 case 2:
  if(DefaultComponentClassId == ForbiddenId)
   return true;

  NetStructure.v.resize(NumLayers.v);
  for(size_t i=0;i<NumLayers.v;i++)
  {
   NetStructure[i].resize(LayerWidth.v);
   for(size_t j=0;j<LayerWidth.v;j++)
   {
	NetStructure[i][j].resize(LayerHeight.v);
	for(size_t k=0;k<LayerHeight.v;k++)
	{
	 NetStructure[i][j][k].resize(ReceptorFieldWidth.v);
	 for(size_t n=0;n<ReceptorFieldWidth.v;n++)
	 {
	  NetStructure[i][j][k][n].resize(ReceptorFieldHeight.v);
	 }
	}
   }
  }

  for(size_t i=0;i<NumLayers.v;i++)
   if(!BuildLayerStructure(i))
    return false;
 break;
 }          */
 return true;
}

// �������� ��������� ���� 'layer'
bool NNet::BuildLayerStructure(size_t layer)
{
 for(size_t i=0;i<LayerWidth.v;i++)
  for(size_t j=0;j<LayerHeight.v;j++)
   if(!BuildReceptorField(layer,i,j))
    return false;

 return true;
}

// �������� ��������� ������������ ���� ���� 'layer', � ������������
// 'i' �� ������, 'j' �� �������
bool NNet::BuildReceptorField(size_t layer, size_t i, size_t j)
{
/* for(size_t n=0;n<ReceptorFieldWidth.v;n++)
  for(size_t m=0;m<ReceptorFieldHeight.v;m++)
  {
   if(!GetComponent(NetStructure[layer][i][j][n][m]))
   {
	NAContainer* comp=InsertComponent();
	if(!comp)
	 return false;

	NetStructure[layer][i][j][n][m]=comp->Id;
   }
  }*/
 return true;
}

// ���������� ����� ����
bool NNet::BuildLinksStructure(void)
{
 for(size_t k=0;k<NumLayers.v;k++)
 {
  for(size_t i=0;i<LayerWidth.v;i++)
   for(size_t j=0;j<LayerHeight.v;j++)
	if(!BuildLinksStructure(k, i, j))
	 return false;
 }
 return true;
}

// ���������� ����� ���������� ���� 'layer', � ������������
// 'i' �� ������, 'j' �� �������
bool NNet::BuildLinksStructure(size_t layer, size_t i, size_t j)
{
 switch(LinksOrganizationMode)
 {
 case 0:
  return BuildLinksStructureNone(layer,i,j);
 case 1:
  return BuildLinksStructureMultilayer(layer,i,j);
 case 2:
  return BuildLinksStructureReceptor(layer,i,j);
 case 3:
  return BuildLinksStructureFullRadius(layer,i,j);
 }
 return false;
}

// ������ ����������� ������ ����, ������������ LinksOrganizationMode
// 0 - ���
bool NNet::BuildLinksStructureNone(size_t layer, size_t i, size_t j)
{
 return true;
}

// ������ ����������� ������ ����, ������������ LinksOrganizationMode
// 1 - ���������� - ��� �� ����� ����� ������ � ����������� 'LinkDistance'
bool NNet::BuildLinksStructureMultilayer(size_t layer, size_t i, size_t j)
{
 return false;
}

// ������ ����������� ������ ����, ������������ LinksOrganizationMode
// 2 - ���������� - ����������� ���� ����� ��������� ������
bool NNet::BuildLinksStructureReceptor(size_t layer, size_t i, size_t j)
{
 return false;
}

// ������ ����������� ������ ����, ������������ LinksOrganizationMode
// 3 - ��� �� ����� � �������� ������� 'LinkDistance'
bool NNet::BuildLinksStructureFullRadius(size_t layer, size_t i, size_t j)
{
 return false;
}
// --------------------------

}
#endif


/*
 * DictProperyManager.cpp
 *
 *  Created on: 2017年12月23日
 *      Author: chengl
 */

#include "DictPropertyManager.h"
#include "TplUtil.h"
#include "DictElement.h"

namespace DictionaryLib
{
std::unique_ptr<DictPropertyManager>  DictPropertyManager::Create()
{
	return CommonUtils::make_unique<DictPropertyManager>();
}


DictPropertyManager::DictPropertyManager()
{

}
DictPropertyManager::~DictPropertyManager()
{

}


void DictPropertyManager::SetFieldValueByPath(std::string path , std::string value)
{
	ElementNode en = ElementManager::Instance()->GetEleNodeByPath(path);
	if(en.GetEleType() != EnumEleType::_primitive)
		THROW(DictionaryException , "only set primitive node.");

	IEleProperty *ptrPeoperty = FindPropertyCache(path);
	if(nullptr == ptrPeoperty)
	{
		ptrPeoperty =  new ElePrimitiveProperty(value);
		InsertEleProperty(path , ptrPeoperty);
	}
	else
	{
		ElePrimitiveProperty *ptrPrimProperty = dynamic_cast<ElePrimitiveProperty*>(ptrPeoperty);
		ptrPrimProperty->SetValue(value);
	}
}
void DictPropertyManager::SetFieldValueByPath(std::string path , long value)
{
	ElementNode en = ElementManager::Instance()->GetEleNodeByPath(path);
	if(en.GetEleType() != EnumEleType::_primitive)
		THROW(DictionaryException , "only set primitive node.");

	IEleProperty *ptrPeoperty = FindPropertyCache(path);
	if(nullptr == ptrPeoperty)
	{
		ptrPeoperty =  new ElePrimitiveProperty(value);
		InsertEleProperty(path , ptrPeoperty);
	}
	else
	{
		ElePrimitiveProperty *ptrPrimProperty = dynamic_cast<ElePrimitiveProperty*>(ptrPeoperty);
		ptrPrimProperty->SetValue(value);
	}
}
void DictPropertyManager::SetFieldValueByPath(std::string path , double value)
{
	ElementNode en = ElementManager::Instance()->GetEleNodeByPath(path);
	if(en.GetEleType() != EnumEleType::_primitive)
		THROW(DictionaryException , "only set primitive node.");

	IEleProperty *ptrPeoperty = FindPropertyCache(path);
	if(nullptr == ptrPeoperty)
	{
		ptrPeoperty =  new ElePrimitiveProperty(value);
		InsertEleProperty(path , ptrPeoperty);
	}
	else
	{
		ElePrimitiveProperty *ptrPrimProperty = dynamic_cast<ElePrimitiveProperty*>(ptrPeoperty);
		ptrPrimProperty->SetValue(value);
	}
}


IEleProperty* DictPropertyManager::FindPropertyCache(std::string path)
{
	auto it = m_eleProperty4Search.find(path);
	if(it == m_eleProperty4Search.end())
		return nullptr;

	return it->second;
}

void DictPropertyManager::InsertEleProperty(std::string path , IEleProperty* eleProPerty)
{
	if( std::string::npos == path.find(PATH_SEP))
	{///第一层节点
		m_elePropertys.push_back(std::shared_ptr<IEleProperty>(eleProPerty));
		m_eleProperty4Search.insert(std::make_pair(path,eleProPerty));
		eleProPerty->SetEleNode(ElementManager::Instance()->GetEleNodeByPath(path));
		return ;
	}

	///多层级
	std::size_t pos =0 , offset =0;
	IEleProperty *ptrParent = nullptr;
	for(int i = 0 ; std::string::npos != (pos = path.find(PATH_SEP , offset)); ++i)
	{
		std::string curPath = path.substr(0 , pos);
		offset = pos + 1;
		fprintf(stdout,"curPath:%s \n", curPath.c_str());
		IEleProperty *ptrProperty = FindPropertyCache(curPath);
		if(nullptr != ptrProperty)
		{
			ptrParent = ptrProperty;
			continue;
		}

		if(i == 0 )
		{//第一层节点
			m_elePropertys.push_back(std::make_shared<EleStructProperty>());
			m_eleProperty4Search.insert(std::make_pair(curPath,m_elePropertys.back().get()));
			m_elePropertys.back()->SetEleNode(ElementManager::Instance()->GetEleNodeByPath(curPath));
			ptrParent = m_elePropertys.back().get();
			continue;
		}
		else
		{
			EleStructProperty* ptrStruct = dynamic_cast<EleStructProperty*>(ptrParent);
			EleStructProperty* ptrCurStruct = new EleStructProperty();
			ptrCurStruct->SetEleNode(ElementManager::Instance()->GetEleNodeByPath(curPath));
			ptrStruct->Insert(curPath , ptrCurStruct);
			m_eleProperty4Search.insert(std::make_pair(curPath,ptrCurStruct));
			ptrParent = ptrCurStruct;
		}
	}
	EleStructProperty* ptrStruct = dynamic_cast<EleStructProperty*>(ptrParent);
	eleProPerty->SetEleNode(ElementManager::Instance()->GetEleNodeByPath(path));
	ptrStruct->Insert(path , eleProPerty);
	m_eleProperty4Search.insert(std::make_pair(path,eleProPerty));
}

void DictPropertyManager::DebugDump()
{
	for(auto it : m_elePropertys)
	{
		it->DebugDump(0);
	}
}

int DictPropertyManager::Encode(char *buffer)
{
	char *p = buffer;
	int tmpSize = 0;
	for(auto it : m_elePropertys)
	{
		tmpSize = it->Encode(p);
		p += tmpSize;
	}

	return p - buffer;
}

}




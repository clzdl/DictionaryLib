/*
 * DictProperyManager.cpp
 *
 *  Created on: 2017年12月23日
 *      Author: chengl
 */

#include "DictPropertyManager.h"
#include "TplUtil.h"
#include "string.h"
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
	if(en.GetEleType() != EnumEleType::_primitive )
		THROW(DictionaryException , "only set primitive node.");

	if(en.GetValueType() != EnumPropertyValueType::_string)
		THROW(DictionaryException , "property value typs not _string.");

	IEleProperty *ptrPeoperty = FindPropertyCache(path);
	if(nullptr == ptrPeoperty)
	{
		ptrPeoperty =  new ElePrimitiveProperty(this,value);
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

	if(en.GetValueType() != EnumPropertyValueType::_long)
		THROW(DictionaryException , "property value typs not _long.");

	IEleProperty *ptrPeoperty = FindPropertyCache(path);
	if(nullptr == ptrPeoperty)
	{
		ptrPeoperty =  new ElePrimitiveProperty(this,value);
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

	if(en.GetValueType() != EnumPropertyValueType::_double)
		THROW(DictionaryException , "property value typs not _double.");

	IEleProperty *ptrPeoperty = FindPropertyCache(path);
	if(nullptr == ptrPeoperty)
	{
		ptrPeoperty =  new ElePrimitiveProperty(this,value);
		InsertEleProperty(path , ptrPeoperty);
	}
	else
	{
		ElePrimitiveProperty *ptrPrimProperty = dynamic_cast<ElePrimitiveProperty*>(ptrPeoperty);
		ptrPrimProperty->SetValue(value);
	}
}


std::string DictPropertyManager::GetFieldValueByPath(std::string path) const
{
	ElementNode en = ElementManager::Instance()->GetEleNodeByPath(path);
	if(en.GetEleType() != EnumEleType::_primitive)
		THROW(DictionaryException , "only set primitive node.");

	IEleProperty *ptrPeoperty = FindPropertyCache(path);
	if(nullptr == ptrPeoperty)
		THROW(DictionaryException , "node property not exists.");

	return dynamic_cast<ElePrimitiveProperty*>(ptrPeoperty)->GetValue();
}

IEleProperty* DictPropertyManager::FindPropertyCache(std::string path) const
{
	auto it = m_eleProperty4Search.find(path);
	if(it == m_eleProperty4Search.end())
		return nullptr;

	return it->second;
}

void DictPropertyManager::Insert2SearchCache(std::string path,IEleProperty* property)
{
	m_eleProperty4Search.insert(std::make_pair(path,property));
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
			m_elePropertys.push_back(std::make_shared<EleStructProperty>(this));
			m_eleProperty4Search.insert(std::make_pair(curPath,m_elePropertys.back().get()));
			m_elePropertys.back()->SetEleNode(ElementManager::Instance()->GetEleNodeByPath(curPath));
			ptrParent = m_elePropertys.back().get();
			continue;
		}
		else
		{
			EleStructProperty* ptrStruct = dynamic_cast<EleStructProperty*>(ptrParent);
			EleStructProperty* ptrCurStruct = new EleStructProperty(this);
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

void DictPropertyManager::DebugDump() const
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

void DictPropertyManager::Decode(char *buffer)
{
	char *p = buffer;
	int tmpAvpCode = 0;
	int tmpLen = 0;
	ElementNode eleNode;
	while(*p)
	{
		memcpy(&tmpAvpCode ,p , TAG_SIZE);
		fprintf(stdout , "avpCode: %d \n" , tmpAvpCode);
		eleNode = ElementManager::Instance()->GetEleNodeByCode(tmpAvpCode);
		if(eleNode.GetEleType() == EnumEleType::_primitive)
		{

			ElePrimitiveProperty *primitiveProperty =  new ElePrimitiveProperty(this);
			primitiveProperty->SetEleNode(eleNode);
			tmpLen = primitiveProperty->Decode(p);
			m_elePropertys.push_back(std::shared_ptr<IEleProperty>(primitiveProperty));
		}
		else if(eleNode.GetEleType() == EnumEleType::_struct)
		{
			EleStructProperty *structPropery = new EleStructProperty(this);
			structPropery->SetEleNode(eleNode);
			tmpLen = structPropery->Decode(p);
			m_elePropertys.push_back(std::shared_ptr<IEleProperty>(structPropery));
		}
		else
		{
			THROW(DictionaryException , "element type wrong.");
		}

		p += tmpLen;
	}
}

}




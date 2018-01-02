/*
 * DictEleNode.cpp
 *
 *  Created on: 2017年12月23日
 *      Author: chengl
 */
#include "NumberUtil.h"
#include <string.h>
#include <memory>
#include "../include/DictEleProperty.h"
#include "StringUtil.h"
#include "../include/DictPropertyManager.h"

namespace DictionaryLib
{
ElePropertyValue::ElePropertyValue(std::string value)
:m_eleValueType(EnumPropertyValueType::_string),
 m_value(value)
{

}
ElePropertyValue::ElePropertyValue(long value)
:m_eleValueType(EnumPropertyValueType::_long),
 m_value(CommonUtils::NumberUtil::Number2String(value))
{

}
ElePropertyValue::ElePropertyValue(double value)
:m_eleValueType(EnumPropertyValueType::_double),
 m_value(CommonUtils::NumberUtil::Float2String(value))
{

}
ElePropertyValue::~ElePropertyValue()
{

}

void ElePropertyValue::SetValue(std::string value)
{
	m_value = value;
}
void ElePropertyValue::SetValue(long value)
{
	m_value = CommonUtils::NumberUtil::Number2String(value);
}
void ElePropertyValue::SetValue(double value)
{
	m_value = CommonUtils::NumberUtil::Float2String(value);
}

std::string ElePropertyValue::GetValue() const
{
	return m_value;
}
EnumPropertyValueType ElePropertyValue::GetValueType() const
{
	return m_eleValueType;
}

void IEleProperty::SetEleNode(const ElementNode& eleNode)
{
	m_eleNode = eleNode;
}

void IEleProperty::SetParent(IEleProperty *parent)
{
	m_ptrParent = parent;
}

void IEleProperty::SerPropertyManager(DictPropertyManager *manager)
{
	m_propertyManager = manager;
}

ElePrimitiveProperty::ElePrimitiveProperty(DictPropertyManager *manager,std::string value)
:IEleProperty(EnumEleType::_primitive,manager),
 m_eleValue(value)
{

}
ElePrimitiveProperty::ElePrimitiveProperty(DictPropertyManager *manager,long value)
:IEleProperty(EnumEleType::_primitive,manager),
 m_eleValue(value)
{

}
ElePrimitiveProperty::ElePrimitiveProperty(DictPropertyManager *manager,double value)
:IEleProperty(EnumEleType::_primitive,manager),
 m_eleValue(value)
{
}

ElePrimitiveProperty::ElePrimitiveProperty(DictPropertyManager *manager)
:IEleProperty(EnumEleType::_primitive,manager),
 m_eleValue("0")
{

}

ElePrimitiveProperty::~ElePrimitiveProperty()
{

}
void ElePrimitiveProperty::SetValue(std::string value)
{
	m_eleValue.SetValue(value);
}
void ElePrimitiveProperty::SetValue(long value)
{
	m_eleValue.SetValue(value);
}
void ElePrimitiveProperty::SetValue(double value)
{
	m_eleValue.SetValue(value);
}

std::string ElePrimitiveProperty::GetValue() const
{
	return m_eleValue.GetValue();
}

int ElePrimitiveProperty::Encode(char *buffer )
{
	char *p = buffer;
	int avpCode = m_eleNode.GetAvpCode();
	memcpy(p , (char*)&avpCode , TAG_SIZE);
	p += TAG_SIZE;

	switch(m_eleValue.GetValueType())
	{
		case EnumPropertyValueType::_double:
		{
			double tmp = atof(m_eleValue.GetValue().c_str());
			memcpy(p , (char*)DOUBLE_SIZE , LEN_SIZE);
			p += LEN_SIZE;
			memcpy(p , (char*)&tmp , DOUBLE_SIZE);
			p += DOUBLE_SIZE;
			break;
		}
		case EnumPropertyValueType::_long:
		{
			long tmp = atol(m_eleValue.GetValue().c_str());
			memcpy(p , (char*)LONG_SIZE , LEN_SIZE);
			p += LEN_SIZE;
			memcpy(p , (char*)&tmp , LONG_SIZE);
			p += LONG_SIZE;
			break;
		}
		case EnumPropertyValueType::_string:
		{
			int size = m_eleValue.GetValue().length();
			memcpy(p , (char*)&size , LEN_SIZE);
			p += LEN_SIZE;
			memcpy(p , m_eleValue.GetValue().c_str() ,size);
			p += size;
			break;
		}
		default:
			THROW(DictionaryException , "value type is wrong.");
	}

//	char hex[1024] = {0};
//	fprintf(stdout," ===:%s \n" , CommonUtils::StringUtil::toHexString(buffer, hex ,  p - buffer));

	return p - buffer;
}

int ElePrimitiveProperty::Decode(char *buffer, IEleProperty* parent)
{
	char *p = buffer;
	int avpCode = 0 , len = 0;

	memcpy(&avpCode ,p , TAG_SIZE);
	p += TAG_SIZE;

	memcpy(&len ,p , LEN_SIZE);
	p += LEN_SIZE;

	if(m_eleNode.GetAvpCode() == 0)
	{
		SetEleNode(ElementManager::Instance()->GetEleNodeByCode(avpCode));
	}
	switch(m_eleNode.GetValueType())
	{
		case EnumPropertyValueType::_double:
		{
			double tmpValue = 0.0;
			memcpy(&tmpValue , p , DOUBLE_SIZE);
			p += DOUBLE_SIZE;
			SetValue(tmpValue);
			break;
		}
		case EnumPropertyValueType::_long:
		{
			long tmpValue = 0;
			memcpy(&tmpValue , p , LONG_SIZE);
			p += LONG_SIZE;
			SetValue(tmpValue);
			break;
		}
		case EnumPropertyValueType::_string:
		{
			std::unique_ptr<char[]> tmpValue(new char[len + 1]());
			memcpy(tmpValue.get() , p , len);
			p += len;
			SetValue(tmpValue.get());
			break;
		}
	}

	if(nullptr != parent)
	{
		SetParent(parent);
		EleStructProperty* ptrStruct = dynamic_cast<EleStructProperty*>(parent);
		ptrStruct->Insert(m_eleNode.GetNodeName() , this);
	}
	m_propertyManager->Insert2SearchCache(m_eleNode.GetPath(),this);
	return p - buffer;
}

void ElePrimitiveProperty::DebugDump(int level)
{
	std::string prefix;
	for(int i = 0; i < level*4 ; ++i)
		prefix.append("-");

	fprintf(stdout , "%s nodeType:%d , value:%s \n" , prefix.c_str(),m_eleType , m_eleValue.GetValue().c_str());
}

EleStructProperty::EleStructProperty(DictPropertyManager *manager)
:IEleProperty(EnumEleType::_struct,manager)
{
}
EleStructProperty::~EleStructProperty()
{
	for(auto it : m_child)
	{
		if(nullptr != it.second)
		{
			delete it.second;
			it.second = nullptr;
		}
	}
	m_child.clear();
}
void EleStructProperty::Insert(std::string nodeName , IEleProperty *eleProperty)
{
	m_child.insert(std::make_pair(nodeName,eleProperty));
}

int EleStructProperty::Encode(char *buffer )
{
	char *p = buffer;

	int avpCode = m_eleNode.GetAvpCode();
	memcpy(p , (char*)&avpCode , TAG_SIZE);
	p += TAG_SIZE;

	int tmpSize = 0,tmpTotalSize = 0;
	char tmpBuffer[4096] = {0};
	char *q = tmpBuffer;
	for(auto it : m_child)
	{
		tmpSize = it.second->Encode(q);

		q += tmpSize;
		tmpTotalSize += tmpSize;
	}

	memcpy(p , (char*)&tmpTotalSize, LEN_SIZE);
	p += LEN_SIZE;

	memcpy(p , tmpBuffer, tmpTotalSize);
	p += tmpTotalSize;

	return p - buffer;
}

int EleStructProperty::Decode(char *buffer, IEleProperty* parent)
{
	char *p = buffer;
	int avpCode = 0 , len = 0;

	memcpy(&avpCode ,p , TAG_SIZE);
	p += TAG_SIZE;

	memcpy(&len ,p , LEN_SIZE);
	p += LEN_SIZE;

	if(m_eleNode.GetAvpCode() == 0)
	{
		SetEleNode(ElementManager::Instance()->GetEleNodeByCode(avpCode));
	}

	if(nullptr != parent)
	{
		SetParent(parent);
		EleStructProperty* ptrStruct = dynamic_cast<EleStructProperty*>(parent);
		ptrStruct->Insert(m_eleNode.GetNodeName() , this);
	}

	int tmpAvpCode = 0;
	int tmpLen = 0;
	int decodeLen = 0;
	ElementNode eleNode;
	fprintf(stdout , "len: %d \n" , len);
	while(decodeLen < len)
	{
		memcpy(&tmpAvpCode ,p , TAG_SIZE);
		fprintf(stdout , "avpCode: %d \n" , tmpAvpCode);
		eleNode = ElementManager::Instance()->GetEleNodeByCode(tmpAvpCode);
		if(eleNode.GetEleType() == EnumEleType::_primitive)
		{
			ElePrimitiveProperty *primitiveProperty = new ElePrimitiveProperty(m_propertyManager);
			primitiveProperty->SetEleNode(eleNode);
			tmpLen = primitiveProperty->Decode(p , this);
			Insert(eleNode.GetNodeName() ,primitiveProperty);
		}
		else if(eleNode.GetEleType() == EnumEleType::_struct)
		{
			EleStructProperty *structPropery = new EleStructProperty(m_propertyManager);
			structPropery->SetEleNode(eleNode);
			tmpLen = structPropery->Decode(p , this);
			Insert(eleNode.GetNodeName() ,structPropery);
		}
		else
		{
			THROW(DictionaryException , "element type wrong.");
		}

		p += tmpLen;
		decodeLen += tmpLen;

		fprintf(stdout , "decodeLen: %d \n" , decodeLen);
	}

	if(decodeLen != len)
		THROW(DictionaryException , "decoded len not equal content len.");

	m_propertyManager->Insert2SearchCache(m_eleNode.GetPath() , this);

	return p - buffer;
}

void EleStructProperty::DebugDump(int level)
{
	std::string prefix;
	for(int i = 0; i < level*4 ; ++i)
		prefix.append("-");

	fprintf(stdout , "%s nodeType:%d \n" , prefix.c_str(),m_eleType );

	for(auto it : m_child)
	{
		it.second->DebugDump(level+1);
	}

}

}



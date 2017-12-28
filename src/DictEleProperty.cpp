/*
 * DictEleNode.cpp
 *
 *  Created on: 2017年12月23日
 *      Author: chengl
 */
#include "NumberUtil.h"
#include <string.h>
#include "../include/DictEleProperty.h"
#include "StringUtil.h"
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

ElePrimitiveProperty::ElePrimitiveProperty(std::string value)
:IEleProperty(EnumEleType::_primitive),
 m_eleValue(value)
{

}
ElePrimitiveProperty::ElePrimitiveProperty(long value)
:IEleProperty(EnumEleType::_primitive),
 m_eleValue(value)
{

}
ElePrimitiveProperty::ElePrimitiveProperty(double value)
:IEleProperty(EnumEleType::_primitive),
 m_eleValue(value)
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

void ElePrimitiveProperty::DebugDump(int level)
{
	std::string prefix;
	for(int i = 0; i < level*4 ; ++i)
		prefix.append("-");

	fprintf(stdout , "%s nodeType:%d , value:%s \n" , prefix.c_str(),m_eleType , m_eleValue.GetValue().c_str());
}

EleStructProperty::EleStructProperty()
:IEleProperty(EnumEleType::_struct)
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



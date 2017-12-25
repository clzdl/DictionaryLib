/*
 * DictEleNode.cpp
 *
 *  Created on: 2017年12月23日
 *      Author: chengl
 */
#include "NumberUtil.h"
#include "../include/DictEleProperty.h"

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



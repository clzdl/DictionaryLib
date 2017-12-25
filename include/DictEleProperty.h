/*
 * DictEleNode.h
 *
 *  Created on: 2017年12月23日
 *      Author: chengl
 */

#ifndef INCLUDE_DICTELEPROPERTY_H_
#define INCLUDE_DICTELEPROPERTY_H_

/**
 *  内存中元素属性结构
 */

#include <string>
#include <list>
#include <map>
#include "DictElement.h"

namespace DictionaryLib
{

enum class EnumPropertyValueType
{
	_string,
	_long,
	_double
};

//元素的具体值
class ElePropertyValue
{
public:
	ElePropertyValue(std::string value);
	ElePropertyValue(long value);
	ElePropertyValue(double value);
	~ElePropertyValue();

	void SetValue(std::string value);
	void SetValue(long value);
	void SetValue(double value);

	std::string GetValue() const;

private:
	std::string m_value;
	EnumPropertyValueType m_eleValueType;
};

class IEleProperty
{
public:
	IEleProperty(EnumEleType eleType)
	:m_eleType(eleType),
	 m_ptrParent(nullptr)
	{}

	virtual ~IEleProperty(){}

	virtual void DebugDump(int level) = 0;
protected:
	EnumEleType m_eleType;
	IEleProperty *m_ptrParent;		///指向父节点
	std::list<IEleProperty*>  m_sameSibling;   ///相同路径下的兄弟节点
};

///原子节点
class ElePrimitiveProperty: public IEleProperty
{
public:
	ElePrimitiveProperty(std::string value);
	ElePrimitiveProperty(long value);
	ElePrimitiveProperty(double value);
	~ElePrimitiveProperty();

	void SetValue(std::string value);
	void SetValue(long value);
	void SetValue(double value);

	void DebugDump(int level);
private:
	ElePropertyValue m_eleValue;
};

class EleStructProperty: public IEleProperty
{
public:
	EleStructProperty();
	~EleStructProperty();

	/**
	 * 插入孩子节点
	 */
	void Insert(std::string nodeName , IEleProperty *eleProperty);


	void DebugDump(int level);
private:
	///key is path
	std::map<std::string , IEleProperty*> m_child;    ///孩子节点

};





}


#endif /* INCLUDE_DICTELEPROPERTY_H_ */


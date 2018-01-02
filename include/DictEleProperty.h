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
	EnumPropertyValueType GetValueType() const;
private:
	std::string m_value;
	EnumPropertyValueType m_eleValueType;
};

class DictPropertyManager;
class IEleProperty
{
public:
	IEleProperty(EnumEleType eleType,DictPropertyManager *manager)
	:m_eleType(eleType),
	 m_ptrParent(nullptr),
	 m_propertyManager(manager)
	{}

	virtual ~IEleProperty(){}
	virtual void DebugDump(int level) = 0;
	/**
	 * 编码，返回值为编码后长度
	 */
	virtual int Encode(char *buffer ) = 0;
	/**
	 * 解码，返回解码的属性节点
	 */
	virtual int Decode(char *buffer , IEleProperty* parent = nullptr) = 0;

	void SetEleNode(const ElementNode& eleNode);

	void SetParent(IEleProperty *parent);

	void SerPropertyManager(DictPropertyManager *manager);
protected:
	EnumEleType m_eleType;
	IEleProperty *m_ptrParent;		///指向父节点
	std::list<IEleProperty*>  m_sameSibling;   ///相同路径下的兄弟节点
	ElementNode m_eleNode;		///指向配置节点指针
	DictPropertyManager *m_propertyManager;
};

///原子节点
class ElePrimitiveProperty: public IEleProperty
{
public:
	ElePrimitiveProperty(DictPropertyManager *manager,std::string value);
	ElePrimitiveProperty(DictPropertyManager *manager,long value);
	ElePrimitiveProperty(DictPropertyManager *manager,double value);
	ElePrimitiveProperty(DictPropertyManager *manager);
	~ElePrimitiveProperty();

	void SetValue(std::string value);
	void SetValue(long value);
	void SetValue(double value);

	std::string GetValue() const;

	void DebugDump(int level);

	int Encode(char *buffer );

	int Decode(char *buffer, IEleProperty* parent = nullptr);
private:
	ElePropertyValue m_eleValue;
};

class EleStructProperty: public IEleProperty
{
public:
	EleStructProperty(DictPropertyManager *manager);
	~EleStructProperty();

	/**
	 * 插入孩子节点
	 */
	void Insert(std::string nodeName , IEleProperty *eleProperty);

	int Encode(char *buffer );
	int Decode(char *buffer, IEleProperty* parent = nullptr);
	void DebugDump(int level);
private:
	///key is path
	std::map<std::string , IEleProperty*> m_child;    ///孩子节点

};





}


#endif /* INCLUDE_DICTELEPROPERTY_H_ */


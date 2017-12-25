/*
 * DictPropertyManager.h
 *
 *  Created on: 2017年12月23日
 *      Author: chengl
 */

#ifndef INCLUDE_DICTPROPERTYMANAGER_H_
#define INCLUDE_DICTPROPERTYMANAGER_H_

#include "DictEleProperty.h"
#include <memory>
#include <vector>

namespace DictionaryLib
{

class DictPropertyManager
{
public:
	static std::unique_ptr<DictPropertyManager>  Create();
	DictPropertyManager();
	~DictPropertyManager();

	/**
	 * 从字符串流解码成层级内存结构，存入m_elePropertys
	 */
	void Decode(const char *buffer);

	/**
	 *  按路径设置原子字段
	 */
	void SetFieldValueByPath(std::string path , std::string value);
	void SetFieldValueByPath(std::string path , long value);
	void SetFieldValueByPath(std::string path , double value);


	/**
	 * 按路径获取原子字段值
	 */
	std::string GetFieldValueByPath(std::string path);

	/**
	 * 按路径获取属性节点
	 */
	std::unique_ptr<IEleProperty> GetElePropertyByPath(std::string path);


	/**
	 * 输出层级内存结构m_elePropertys成字符串流
	 */
	void Encode(char *buffer);

	/**
	 * 内存结构调试输出
	 */
	void DebugDump();

private:
	DictPropertyManager(const DictPropertyManager &dpm) = delete;
	DictPropertyManager& operator =(const DictPropertyManager &dpm) = delete;
	DictPropertyManager(DictPropertyManager *&dpm) = delete;
	DictPropertyManager& operator =(DictPropertyManager &&dpm) = delete;

	/**
	 * 查找内存中节点是否已存在
	 */
	IEleProperty* FindPropertyCache(std::string path);

	/**
	 * 插入属性值
	 */
	void InsertEleProperty(std::string path , IEleProperty* eleProPerty);


	///解码后的字段层级内存缓冲区
	std::vector<std::shared_ptr<IEleProperty> > m_elePropertys;
	//解码字段查找缓冲区
	std::unordered_map<std::string , IEleProperty* > m_eleProperty4Search;
};


}



#endif /* INCLUDE_DICTPROPERTYMANAGER_H_ */

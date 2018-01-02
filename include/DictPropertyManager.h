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
	 *  按路径设置原子字段
	 */
	void SetFieldValueByPath(std::string path , std::string value , bool overlap = true);
	void SetFieldValueByPath(std::string path , long value , bool overlap = true);
	void SetFieldValueByPath(std::string path , double value , bool overlap = true);


	/**
	 * 按路径获取原子字段值
	 */
	std::string GetFieldValueByPath(std::string path, int pos = 0) const;

	/**
	 * 查找内存中节点是否已存在
	 */
	IEleProperty* FindPropertyCache(std::string path) const;


	/**
	 * 输出层级内存结构m_elePropertys成字符串流,输出为编码后的长度
	 */
	int Encode(char *buffer);

	/**
	 * 解码，形成层级内存结构
	 */
	void Decode(char *buffer);

	/**
	 * 内存结构调试输出
	 */
	void DebugDump() const;


	void Insert2SearchCache(std::string path ,IEleProperty* property);

private:
	DictPropertyManager(const DictPropertyManager &dpm) = delete;
	DictPropertyManager& operator =(const DictPropertyManager &dpm) = delete;
	DictPropertyManager(DictPropertyManager *&dpm) = delete;
	DictPropertyManager& operator =(DictPropertyManager &&dpm) = delete;


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

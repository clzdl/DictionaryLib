/*
 * DictElement.h
 *
 *  Created on: 2017年12月23日
 *      Author: chengl
 */

#ifndef INCLUDE_DICTELEMENT_H_
#define INCLUDE_DICTELEMENT_H_

/**
 * 字典的配置元素
 */
#include <string>
#include <unordered_map>
#include "Exception.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeIterator.h"
#include "Poco/DOM/NodeFilter.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/DOM/NamedNodeMap.h"
#include "Poco/Exception.h"
#include "Poco/AutoPtr.h"

#define 	PATH_SEP	"."				//路径层级分割符

namespace DictionaryLib
{

DECLARE_EXCEPTION(DictionaryException, CommonUtils::Exception);

enum class EnumEleType
{
	_primitive,
	_struct,
};

class ElementNode
{
public:
	ElementNode(EnumEleType eleType);
	~ElementNode();
	ElementNode(const ElementNode &en) = default;
	ElementNode& operator =(const ElementNode &en) = default;
	ElementNode(ElementNode &&en) = default;
	ElementNode& operator =(ElementNode &&en) = default;

	EnumEleType GetEleType() const;
	void SetPath(std::string path);
	std::string GetPath() const;
	void SetNodeName(std::string name);
	std::string GetNodeName() const;
	void SetAvpCode(std::size_t avpCode);
	std::size_t GetAvpCode() const;
	void SetFuncString(std::string funcString);
	std::string GetFuncString() const;
	void SetOutType(bool outType);
	bool GetOutType() const;
	void SetParentPtr(ElementNode *ptr);
	const ElementNode* GetParentPtr() const;

private:
	EnumEleType m_eleType;
	std::string m_path;   ///节点路径
	std::string m_nodeName;    ///节点名称
	std::size_t m_avpCode;		///avpCode码
	std::string m_funcString;			///字段解码函数(暂用作数据类型：string/long/double)
	bool m_outType;				///是否输出标识
	ElementNode *m_ptrParent;    ///指向父节点的指针
};

///字典元素管理器
class ElementManager
{
public:
	static ElementManager* Instance(std::string configFile = "");
	/**
	 * 根据路径获取字典配置信息
	 */
	ElementNode GetEleNodeByPath(std::string path);

	/**
	 * 输出内存信息
	 */
	void DebugDump();

private:
	ElementManager(std::string configFile) ;
	ElementManager(const ElementManager &em) = delete;
	ElementManager& operator = (const ElementManager &em) = delete;
	ElementManager(ElementManager &&em) = delete;
	ElementManager& operator = (ElementManager &&em) = delete;

	/**
	 * 加载配置文件
	 */
	void LoadConfig();
	/**
	 * 读取节点信息
	 */
	ElementNode CreateEleNode(Poco::XML::Node *xmlNode);

	/**
	 * 读取节点信息
	 */
	void ReadXmlNode(Poco::XML::Node *xmlNode, ElementNode* eleParent = nullptr);

	/**
	 * 输出内存节点调试信息
	 */
	void DebugEleNode(const ElementNode *eleNode , int level = 0);

	static ElementManager *m_objInstance;
	std::string m_configFile;
	//key is path
	std::unordered_map<std::string , ElementNode> m_eleContainer;
};

}



#endif /* INCLUDE_DICTELEMENT_H_ */

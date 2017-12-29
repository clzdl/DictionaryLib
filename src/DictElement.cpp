/*
 * DictElement.cpp
 *
 *  Created on: 2017年12月23日
 *      Author: chengl
 */

#include "DictElement.h"
#include <memory>
#include <assert.h>


namespace DictionaryLib
{
ElementManager* ElementManager::m_objInstance = nullptr;

ElementNode::ElementNode()
:m_eleType(EnumEleType::_primitive),
 m_avpCode(0),
 m_outType(true),
 m_ptrParent(nullptr)
{

}

ElementNode::ElementNode(EnumEleType eleType)
:m_eleType(eleType),
 m_avpCode(0),
 m_outType(true),
 m_ptrParent(nullptr)
{
}
ElementNode::~ElementNode()
{
}
EnumEleType ElementNode::GetEleType() const
{
	return m_eleType;
}

void ElementNode::SetPath(std::string path)
{
	m_path = path;
}
std::string ElementNode::GetPath() const
{
	return m_path;
}
void ElementNode::SetNodeName(std::string name)
{
	m_nodeName = name;
}
std::string ElementNode::GetNodeName() const
{
	return m_nodeName;
}
void ElementNode::SetAvpCode(std::size_t avpCode)
{
	m_avpCode = avpCode;
}
std::size_t ElementNode::GetAvpCode() const
{
	return m_avpCode;
}
void ElementNode::SetFuncString(std::string funcString)
{
	m_funcString = funcString;
}
std::string ElementNode::GetFuncString() const
{
	return m_funcString;
}
void ElementNode::SetOutType(bool outType)
{
	m_outType = outType;
}
bool ElementNode::GetOutType() const
{
	return m_outType;
}
void ElementNode::SetValueType(EnumPropertyValueType valueType)
{
	m_valueType = valueType;
}
EnumPropertyValueType ElementNode::GetValueType() const
{
	return m_valueType;
}

void ElementNode::SetParentPtr(ElementNode *ptr)
{
	m_ptrParent = ptr;
}
const ElementNode* ElementNode::GetParentPtr() const
{
	return m_ptrParent;
}

ElementManager* ElementManager::Instance(std::string configFile)
{
	if(nullptr == m_objInstance)
	{
		m_objInstance = new ElementManager(configFile);
		m_objInstance->LoadConfig();
	}
	return m_objInstance;
}

ElementManager::ElementManager(std::string configFile)
:m_configFile(configFile)
{

}

void ElementManager::LoadConfig()
{
	Poco::XML::InputSource src(m_configFile);
	try
	{
		Poco::XML::DOMParser parser;
		Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parse(&src);
		Poco::XML::NodeIterator it(pDoc, Poco::XML::NodeFilter::SHOW_ELEMENT);
		Poco::XML::Node *pRoot = it.nextNode();
		if( pRoot->nodeName().compare( "config") )
			THROW(DictionaryException, "dictionary config xml configure wrong,must be started with config.");

		if(!pRoot->hasChildNodes())
			THROW(DictionaryException, "dictionary config is empty.");

		Poco::XML::NodeList *nl = pRoot->childNodes();
		for(int i = 0 ; i < nl->length(); ++i)
		{
			Poco::XML::Node *ptrNode = nl->item(i);
			ReadXmlNode(ptrNode);
		}
	}
	catch (Poco::Exception& e)
	{
		THROW(DictionaryException, e.displayText());
	}

}


ElementNode ElementManager::GetEleNodeByPath(std::string path)
{
	auto it = m_eleContainer.find(path);
	if(it == m_eleContainer.end())
		THROW_P1(DictionaryException , "path[%s] not exists." , path.c_str());

	return it->second;
}

ElementNode ElementManager::GetEleNodeByCode(int avpCode)
{
	auto it = m_avpCode2KeyPathRel.find(avpCode);
	if(it == m_avpCode2KeyPathRel.end())
		THROW_P1(DictionaryException , "code[%d] not exists.", avpCode);


	return GetEleNodeByPath(it->second);
}

void ElementManager::ReadXmlNode(Poco::XML::Node *xmlNode, ElementNode* eleParent)
{
	if(Poco::XML::Node::ELEMENT_NODE != xmlNode->nodeType())
		return;

	ElementNode tmpEleNode = CreateEleNode(xmlNode);
	tmpEleNode.SetParentPtr(eleParent);
	tmpEleNode.SetPath(nullptr == eleParent?tmpEleNode.GetNodeName() : eleParent->GetPath()+"."+tmpEleNode.GetNodeName());

	auto it = m_eleContainer.insert(std::make_pair(tmpEleNode.GetPath() , tmpEleNode));
	ElementNode &eleNode = it.first->second;

	if(eleNode.GetEleType() == EnumEleType::_struct)
	{
		if(!xmlNode->hasChildNodes())
		{
			THROW(DictionaryException , "xml node configure  wrong. node type is struct,but havn't child");
		}
		Poco::XML::NodeList *nl = xmlNode->childNodes();
		for(int i = 0; i < nl->length(); ++i)
		{
			ReadXmlNode(nl->item(i) , &eleNode);
		}
	}

}

ElementNode ElementManager::CreateEleNode(Poco::XML::Node *xmlNode)
{
	Poco::XML::NamedNodeMap* nodeAttr = xmlNode->attributes();
	Poco::XML::Node *nodeType = nodeAttr->getNamedItem("node_type");
	assert(NULL != nodeType);
	ElementNode eleNode(nodeType->nodeValue().compare("composite")==0?EnumEleType::_struct:EnumEleType::_primitive);
	eleNode.SetNodeName(xmlNode->nodeName());

	Poco::XML::Node *nodeAvpCode = nodeAttr->getNamedItem("avp_code");
	assert(nullptr != nodeAvpCode);
	eleNode.SetAvpCode(atol(nodeAvpCode->nodeValue().c_str()));

	if(eleNode.GetEleType() == EnumEleType::_primitive)
	{
		Poco::XML::Node *nodeFuncString = nodeAttr->getNamedItem("func");
		assert(nullptr != nodeFuncString);
		eleNode.SetFuncString(nodeFuncString->nodeValue());

		if(eleNode.GetFuncString() == "_string")
		{
			eleNode.SetValueType(EnumPropertyValueType::_string);
		}
		else if(eleNode.GetFuncString() == "_long")
		{
			eleNode.SetValueType(EnumPropertyValueType::_long);
		}
		else if(eleNode.GetFuncString() == "_double")
		{
			eleNode.SetValueType(EnumPropertyValueType::_double);
		}
		else
		{
			THROW_P1(DictionaryException , "prop func[%s] wrong.only support[_string/_long/_double]",eleNode.GetFuncString().c_str());
		}


		Poco::XML::Node *nodeOutType = nodeAttr->getNamedItem("out_type");
		assert(nullptr != nodeOutType);
		eleNode.SetOutType(nodeOutType->nodeValue().compare("true")==0?true:false);
	}

	return eleNode;
}

void ElementManager::DebugDump()
{
	for(const auto &it : m_eleContainer)
	{
		if(nullptr == it.second.GetParentPtr())
			DebugEleNode(&(it.second));
	}
}

void ElementManager::DebugEleNode(const ElementNode *eleNode ,int level)
{
	std::string prefix;
	for(int i = 0; i < level*4 ; ++i)
		prefix.append("-");

	fprintf(stdout , "%s path:%s\n" , prefix.c_str() , eleNode->GetPath().c_str());
	fprintf(stdout , "%s eleType:%d\n" , prefix.c_str() , eleNode->GetEleType());
	fprintf(stdout , "%s nodeName:%s\n" , prefix.c_str() , eleNode->GetNodeName().c_str());
	fprintf(stdout , "%s avpCode:%d\n" , prefix.c_str() , eleNode->GetAvpCode());
	fprintf(stdout , "%s funString:%s\n" , prefix.c_str() , eleNode->GetFuncString().c_str());
	fprintf(stdout , "%s outType:%d\n" , prefix.c_str() , eleNode->GetOutType());

	if(eleNode->GetEleType() == EnumEleType::_struct)
	{
		for(const auto &it : m_eleContainer)
		{
			if(it.second.GetParentPtr() == eleNode)
			{
				DebugEleNode(&(it.second) , level + 1);
			}
		}
	}

}

}


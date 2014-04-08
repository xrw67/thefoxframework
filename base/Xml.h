#ifndef _THEFOX_BASE_XML_H_
#define _THEFOX_BASE_XML_H_

#include <base/Types.h>
#include <thirdparty/jsoncpp/json.h>
#include <thirdparty/tinyxml/tinyxml.h>

namespace thefox
{

void xml2json(const TiXmlElement *xml, Json::Value &json)
{
	// 标签名
	json[xml.Value()] = xml.GetText();
	// 属性
	TiXmlAttribute *xmlAttr = xml.FirstAttribute();
	while (xmlAttr) {
		json[String("-")+xmlAttr->Name()] = xmlAttr->Value();
		xmlAttr->Next();
	}
	// 子元素
	TiXmlElement *child = xml.FirstChildElement();
	while (child) {
		Json::Value jsonChild;
		xml2json(child, jsonChild);
		json.append(jsonChild);
		child->NextSiblingElement();
	}
}

void xml2json(const String &xmlStr, String &jsonStr)
{
	TiXmlDocument xml();
	xml.Parse(xmlStr);

	Json::Writer json;
	Json::Value root;
	xml2json(xml.RootElement(), root);
	jsonStr = writer.write(root);
}

void Json2xml(const Json::Value *json, TiXmlElement xml)
{
	for (Json::Value::const_iterator it = json.begin();
			it != json.end(); ++it) {
		
	}
}

void json2xml(const String &jsonStr, const String &xmlStr)
{
	Json::Reader reader;
	Json::Value jsonRoot;
	reader.parse(jsonStr, jsonRoot, false);	
	
	TiXmlElement xmlRoot;
	json2xml(&jsonRoot, xmlRoot);
}

} // namespace thefox

#endif // _THEFOX_BASE_XML_H_

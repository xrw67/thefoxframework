#ifndef _THEFOX_BASE_XML_H_
#define _THEFOX_BASE_XML_H_

#include <base/Types.h>
#include <thirdparty/jsoncpp/json.h>
#include <thirdparty/tinyxml/tinyxml.h>

namespace thefox
{

void xml2json(TiXmlElement *xml, Json::Value &json)
{
	Json::Value children;
	TiXmlElement *child = xml->FirstChildElement();
	while (child) {
		Json::Value jsonChild;
		xml2json(child, jsonChild);
		children.append(jsonChild);
		child = child->NextSiblingElement();
	}
	TiXmlAttribute *xmlAttr = xml->FirstAttribute();
	while (xmlAttr) {
		children[String("-")+xmlAttr->Name()] = xmlAttr->Value();
		xmlAttr = xmlAttr->Next();
	}

	if (children.size())
		json[xml->Value()] = children;
	if (xml->GetText())
		json[xml->Value()] = xml->GetText();
}

void xml2json(const String &xmlStr, String &jsonStr)
{
	TiXmlDocument xml;
	xml.Parse(xmlStr.c_str());

	Json::Value root;
	xml2json(xml.RootElement(), root);

	Json::FastWriter jsonWriter;
	jsonStr = jsonWriter.write(root);
}

} // namespace thefox

#endif // _THEFOX_BASE_XML_H_

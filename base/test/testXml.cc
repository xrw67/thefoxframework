#include <iostream>
#include <base/Xml.h>

using namespace thefox;

int main(int argc, char **argv)
{
	String xmlString("<animals>"
			"<dog><name>Rufus</name><breed>labrador</breed></dog>"
			"<dog><name>Marty</name><breed>whippet</breed></dog>"
			"<cat name=\"Matilda\"/>"
			"</animals>");

	String jsonString;
	xml2json(xmlString, jsonString);
	std::cout << jsonString << "\r\n";
	return 0;
}

#include <snmp/Snmp.h>

using namespace thefox;

int main(int argc, char **argv)
{
	Snmp snmp;
	snmp.open1("127.0.0.1","public");
    
    std::vector<String> result;
    if (snmp.get(".1.3.6.1.2.1.1.2.0,.1.3.6.1.2.1.1.3.0", result))
    {
        
    }
    
    String oids(".1.3.6.1.2");
    while (snmp.getNext(oids, oids, result))
    {
        if (!strncmp(oids.c_str(), "",strlen("")))
            break;
    }
	
	return 0;
}



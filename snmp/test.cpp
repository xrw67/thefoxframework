#include <snmp/Snmp.h>
#include <iostream>
using namespace thefox;

int main(int argc, char **argv)
{
	Snmp snmp;
	if (snmp.open1("192.168.88.96","public"))
	{
		std::cout<<"open1 success"<<std::endl;
	}
	else
	{
		std::cout<<"open1 fail"<<std::endl;
	}
    
	String oid = ".1.3.6.1.2.1.2.2.1.6.2";
	//String oid = ".1.3.6.1.2.1.1.2.0";
	String value;
	std::cout<<"get one start-------------"<<std::endl;
	if (snmp.get(oid, value))
    {
		std::cout<<"oid:"<<oid<<" value:"<<value<<std::endl;
    }
	else
	{
		std::cout<<snmp.GetLastErrorStr()<<std::endl;
	}
	std::cout<<"get one stop-------------"<<std::endl;
	
	std::cout<<"get many start-------------"<<std::endl;
	OidVector oids;
    ValueVector result;
	oids.push_back(".1.3.6.1.2.1.1.1.0");
	oids.push_back(".1.3.6.1.2.1.1.2.0");
	oids.push_back(".1.3.6.1.2.1.1.3.0");
	
    if (snmp.get(oids, result))
    {
        for (int i=0; i < result.size(); ++i)
		{
			std::cout<<"oid:"<<oids[i]<<" value:"<<result[i]<<std::endl;
		}
    }
	else
	{
		std::cout<<snmp.GetLastErrorStr()<<std::endl;
	}
    std::cout<<"get many stop-------------"<<std::endl;
	
	std::cout<<"getnext one start-------------"<<std::endl;
	oid = ".1.3.6.1.2.1.2.2.1.1";
    while (snmp.getNext(oid, value))
    {
        if (strncmp(oid.c_str(), ".1.3.6.1.2.1.2.2.1.1", strlen(".1.3.6.1.2.1.2.2.1.1")))
            break;
		std::cout<<"oid:"<<oid<<" value:"<<value<<std::endl;
    }
	std::cout<<"getnext one stop-------------"<<std::endl;
	
	
	std::cout<<"getnext many start-------------"<<std::endl;
	oids.clear();
	oids.push_back(".1.3.6.1.2.1.2.2.1.2");
	oids.push_back(".1.3.6.1.2.1.2.2.1.3");
    while (snmp.getNext(oids, result))
    {
        if (strncmp(oids[0].c_str(), ".1.3.6.1.2.1.2.2.1.2", strlen(".1.3.6.1.2.1.2.2.1.2")))
            break;
		for (int i=0; i < result.size(); ++i)
		{
			std::cout<<"oid:"<<oids[i]<<" value:"<<result[i]<<std::endl;
		}
    }
	std::cout<<"getnext many stop-------------"<<std::endl;
	
	std::cout<<"gettable start-------------"<<std::endl;
	oids.clear();
	oids.push_back(".1.3.6.1.2.1.2.2.1.1");
	oids.push_back(".1.3.6.1.2.1.2.2.1.2");
	Table<VB_PTR> values;
	
	if (snmp.getTable(oids, values))
	{
		while (values.fetchRow())
		{
			std::cout<<"oid:"<<values[0]->oid<<" value:"<<values[0]->value<<" | oid:"<<values[1]->oid<<"value:"<<values[1]->value<<std::endl;
		}
	}
	else
	{
		std::cout<<snmp.GetLastErrorStr()<<std::endl;
	}
	std::cout<<"gettable stop-------------"<<std::endl;
	
	
	std::cout<<"snmpset start-------------"<<std::endl;
	SnmpSetInputArray input;
	VB_PTR ptr(new VB);
	ptr->oid = ".1.3.6.1.2.1.1.5.0";//oid
	ptr->type = SNMP_TYPE_STRING;
	ptr->value = "srdit.net";
	input.push_back(ptr);

	// String inOid = ".1.3.6.1.2.1.1.5.0";
	// char type = SNMP_TYPE_STRING;//'s'
	// String value = "linux";
	if (snmp.set(input))
	//if (snmp.set(inOid, type, value))
	{
		std::cout<<"snmpset success"<<std::endl;
	}
	else
	{
		std::cout<<snmp.GetLastErrorStr()<<std::endl;
	}
	std::cout<<"snmpset stop-------------"<<std::endl;
	
	std::cout<<"over"<<std::endl;
	return 0;
}



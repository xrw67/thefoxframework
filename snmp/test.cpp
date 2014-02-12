#incldue <snmp/snmp.h>

using namespace thefox;

void testGet(Snmp &snmp)
{
	Pdu pdu;
	pdu.add(Vb(.1.3.6.1.2.1.1.2.0));
	pdu.add(Vb(.1.3.6.1.2.1.1.3.0));
	
	if (snmp.get(pdu))
	{
		printf(,pdu.)
	}
}

void testGetNext(Snmp &snmp)
{

}



int main(int argc, char **argv)
{
	Snmp snmp;
	snmp.open1(host,community,timeout,retries);
	snmp.open2(host,community,timeout,retries);
	snmp.open3(host,securityName,securityLevel,authProtocol,authPassphrase,privProtocol,privPassphrase,timeout,retries);
	
	return 0;
}



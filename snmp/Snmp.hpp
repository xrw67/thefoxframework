#ifdef _THEFOX_SNMP_H_
#define _THEFOX_SNMP_H_

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

namespace thefox
{

class Snmp
{
public:
	Snmp()
		:_sessionPtr(NULL)
	{}
	
	~Snmp()
	{}
	
	bool open1(const std::string &ipAddr, const std::string &community, const int port = 161, const int retries = 0, const int timeout = 3 * 1000 * 1000)
	{
		return open(ipAddr, community, port, retries, timeout, SNMP_VERSION_1);
	}
	
	bool open2(const std::string &ipAddr, const std::string &community, const int port = 161, const int retries = 0, const int timeout = 3 * 1000 * 1000)
	{ 
		return open(ipAddr, community, port, retries, timeout, SNMP_VERSION_2c);
	}
	
	/// @TODO bool open3(...)
	
	void close()
	{
		if (NULL == _sessionPtr)
		{
			snmp_close(_sessionPtr);
			_sessionPtr = NULL;
		}
	}
	
	bool get(const std::string &oids, std::vector<string> &values)
	{
		if (oids.length() < 1)
			return false;
			
		netsnmp_pdu *pdu;
		netsnmp_pdu *response;
		netsnmp_variable_list *vars;
		
		if (NULL == (pdu = snmp_pdu_create(SNMP_MSG_GET)))
			return false;
		
		// 解析OID
		size_t pos = 0;
		size_t pos2 = 0;
		while (true)
		{
			pos2 = oids.find_first_of(',', pos + 1);
			
			if (std::string::npos == pos2)
		}
		
		for (int l = 0,int r=oids.find; i < )
		{
			name_length = MAX_OID_LEN;
			if (!snmp_parse_oid(,))
				return false;
			else
				snmp_add_null_var(pdu, name, name_length);
		}
		
		int status = snmp_synch_response(_sessionPtr, pdu, response);
		if (STAT_SUCCESS == status)
		{
			if (SNMP_ERR_NOERROR == response->errstat)
			{
				for (vars = response->variables; vars; vars = vars->next_variable)
				{
					
				}
			}
			else
			{
				
			}
		}
		else if (STAT_TIMEOUT == status)
		{
			// 超时
		}
		else // status == STAT_ERROR
		{
			// 失败
			//TRACE(snmp_sess_perror(_sessionPtr));
		}
		
		if (response)
			snmp_free_pdu(response);
	}
	
	bool getNext(const std::string &oids, std::string &newOids, std::vector<string> &values)
	{
	}
	
private:
	bool open(const std::string &ipAddr, 
				const std::string &community,
				const int port,
				const int retries,
				const int timeout,
				const int version)
	{
		char addr[128];
		snprintf(addr, sizeof(addr), "%s:%d", ipAddr.c_str(), port);
		
		snmp_sess_init(&_session);
		_session.version = version; // 版本
		_session.retries = retries; // 重试次数
		_session.timeout =  timeout; // 超时时间 微妙，（net-snmp默认值为1s）
		_session.peername = addr;
		_session.remote_port = port; // 新版不再使用了
		_session.community = community.c_str();
		_session.community_len = community.length();
		
		if (NULL == (ss = snmp_open(&session)))
			return false;
		return true;
	}
	
	struct snmp_session _session;
	struct snmp_session *_sessionPtr;
};

} // nameapace thefox

#endif // _THEFOX_SNMP_H_

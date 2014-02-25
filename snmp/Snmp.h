#ifdef _THEFOX_SNMP_H_
#define _THEFOX_SNMP_H_

#include <vector>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <base/Types.h>
#include <base/noncopyable.h>
#include <log/log.h>

namespace thefox
{

class Snmp : public noncopyable
{
public:
	Snmp()
		:_sessionPtr(NULL)
	{}
	
	~Snmp()
	{}
	
	bool open1(const String &ipAddr,
               const String &community,
               int port = 161,
               int retries = 0,
               int timeout = 3 * 1000 * 1000)
	{
		return open(ipAddr, community, port, retries, timeout, SNMP_VERSION_1);
	}
	
	bool open2(const String &ipAddr,
               const String &community,
               int port = 161,
               int retries = 0,
               int timeout = 3 * 1000 * 1000)
	{ 
		return open(ipAddr, community, port, retries, timeout, SNMP_VERSION_2c);
	}
	
	void close()
	{
		if (NULL == _sessionPtr)
		{
			snmp_close(_sessionPtr);
			_sessionPtr = NULL;
		}
	}
	
	bool get(const String &oids, std::vector<String> &values)
	{
		if (0 == oids.length())
			return false;
			
		netsnmp_pdu *pdu = NULL;
		netsnmp_pdu *response = NULL;
		netsnmp_variable_list *vars = NULL;
		
		if (NULL == (pdu = snmp_pdu_create(SNMP_MSG_GET)))
			return false;
		
		// 解析OID
		size_t pos = 0;
		size_t lastPos = 0;
		while (true)
		{
			pos = oids.find_first_of(',', lastPos);
			if (String::npos == pos)
			{
				pos = oids.length();
				if (pos != lastPos)
				{
					String oidBuf = oids.substr(lastPos, pos - lastPos);
					snmp_add_null_var(pdu, oidBuf.c_str(), oidBuf.length());
				}
				break;
			}
			else
			{
				if (pos != lastPos)
				{
					String oidBuf = oids.substr(lastPos, pos - lastPos);
					snmp_add_null_var(pdu, oidBuf.c_str(), oidBuf.length());
				}
			}
			lastPos = pos + 1;
		}
		
		int status = snmp_synch_response(_sessionPtr, pdu, response);
		if (STAT_SUCCESS == status)
		{
			if (SNMP_ERR_NOERROR == response->errstat)
			{
				values.clear();
				for (vars = response->variables; vars; vars = vars->next_variable)
				{
					values.push_back(asn2String(vars));
				}
			}
			else
			{
				// PDU错误
				return false;
			}
		}
		else if (STAT_TIMEOUT == status)
		{
			// 超时
			
		}
		else // status == STAT_ERROR
		{
			//  ß∞‹
			//TRACE(snmp_sess_perror(_sessionPtr));
			
		}
		
		if (response)
			snmp_free_pdu(response);
		
		return (STAT_SUCCESS == status) ? true : false;
	}
	
	bool getNext(const String &oids, String &newOids, std::vector<string> &values)
	{
		if (0 == oids.length())
			return false;
			
		netsnmp_pdu *pdu = NULL;
		netsnmp_pdu *response = NULL;
		netsnmp_variable_list *vars = NULL;
		
		if (NULL == (pdu = snmp_pdu_create(SNMP_MSG_GET)))
			return false;
		
		// 解析OID
		size_t pos = 0;
		size_t lastPos = 0;
		while (true)
		{
			pos = oids.find_first_of(',', lastPos);
			if (std::string::npos == pos)
			{
				pos = oids.length();
				if (pos != lastPos)
				{
					String oidBuf = oids.substr(lastPos, pos - lastPos);
					snmp_add_null_var(pdu, oidBuf.c_str(), oidBuf.length());
				}
				break;
			}
			else
			{
				if (pos != lastPos)
				{
					String oidBuf = oids.substr(lastPos, pos - lastPos);
					snmp_add_null_var(pdu, oidBuf.c_str(), oidBuf.length());
				}
			}
			lastPos = pos + 1;
		}
		
		int status = snmp_synch_response(_sessionPtr, pdu, response);
		if (STAT_SUCCESS == status)
		{
			if (SNMP_ERR_NOERROR == response->errstat)
			{
				newOids = "";
				values.clear();
				for (vars = response->variables; vars; vars = vars->next_variable)
				{
                    newOids += oid2String(vars->name, vars->name_length);
                    newOids += ",";
					values.push_back(asn2String(vars));
				}
                if (newOids.length() > 0)
                    newOids = newOids.substr(0, newOids.length() - 1);
			}
			else
			{
				// PDU¥ÌŒÛ
				
			}
		}
		else if (STAT_TIMEOUT == status)
		{
			LOG_INFO
			
		}
		else // status == STAT_ERROR
		{
			//  ß∞‹
			//TRACE(snmp_sess_perror(_sessionPtr));
			
		}
		
		if (response)
			snmp_free_pdu(response);
		
		return (STAT_SUCCESS == status) ? true : false;
	}
	
private:
	bool open(const String &ipAddr,
				const String &community,
				int port,
				int retries,
				int timeout,
				int version)
	{
		char addr[128];
		snprintf(addr, sizeof(addr), "%s:%d", ipAddr.c_str(), port);
		
		snmp_sess_init(&_session);
		_session.version = version; // ∞Ê±æ
		_session.retries = retries; // ÷ÿ ‘¥Œ ˝
		_session.timeout =  timeout; // ≥¨ ± ±º‰ Œ¢√Ó£¨£®net-snmpƒ¨»œ÷µŒ™1s£©
		_session.peername = addr;
		_session.remote_port = port; // –¬∞Ê≤ª‘Ÿ π”√¡À
		_session.community = community.c_str();
		_session.community_len = community.length();
		
		if (NULL == (ss = snmp_open(&session)))
			return false;
		return true;
	}
	
    static String oid2String(const oid *oid, int len)
    {
        char str[MAX_OID_LEN] = {0};
        char tmp[32] = {0};
        for (int i = 0; i < len; ++i)
        {
            snprintf(tmp, sizeof(tmp), ".%ld", oid[i]);
            strcat(tsr, tmp);
        }
        return str;
    }
    
	static String asn2String(const netsnmp_variable_list *var)
	{
		
		switch (var->type)
		{
			case ASN_INTEGER:
			{
				char str[32];
				snprintf(str, sizeof(str), "%ld", *var->val.integer);
				return str;
			}
			case ASN_OCTET_STR:
			{
				String str;
				for(int i = 0; i < var->val_len; ++i)
				{
					str += var->val.string[i];
				}
				return str;
			}
			case ASN_BIT_STR:
				
			case ASN_OPAQUE:
				
			case ASN_OBJECT_ID:
				
			case ASN_TIMETICKS:
				
			case ASN_GAUGE:
				
			case ASN_COUNTER:
				
			case ASN_IPADDRESS:
				
			case ASN_NULL:
                return "null";
			case ASN_UINTEGER:
				
			case ASN_COUNTER64:
			case ASN_OPAQUE_U64:
			case ASN_OPAQUE_I64:
			case ASN_OPAQUE_COUNTER64:
			
			case ASN_OPAQUE_FLOAT:
            {
                char buf[32];
                snprintf(buf, sizeof(buf), "%f", vars->);
            }
			case ASN_OPAQUE_DOUBLE:
            {
                char buf[32];
                snprintf(buf, sizeof(buf), "%lf", vars->);
            }
			default:
			
		}
	}
	
	struct snmp_session _session;
	struct snmp_session *_sessionPtr;
};

} // nameapace thefox

#endif // _THEFOX_SNMP_H_

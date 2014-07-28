#ifndef _THEFOX_SNMP_H_
#define _THEFOX_SNMP_H_

#include <base/Types.h>
#include <base/noncopyable.h>
#include <base/Table.h>
#include <memory>
#include <vector>
#include <arpa/inet.h>
//#include <log/logging.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>


namespace thefox {

#define SNMP_TYPE_INTEGER 'i'  
#define SNMP_TYPE_UNSIGNED 'u'  
#define SNMP_TYPE_STRING 's'  
#define SNMP_TYPE_HEX_STRING 'x'  
#define SNMP_TYPE_DECIMAL_STRING 'd'  
#define SNMP_TYPE_NULLOBJ 'n'  
#define SNMP_TYPE_OBJID 'o'  
#define SNMP_TYPE_TIMETICKS 't'  
#define SNMP_TYPE_IPADDRESS 'a'  
#define SNMP_TYPE_BITS 'b'  

struct Vb
{
	string oid;
	char type;
	string value;
};

typedef shared_ptr<struct Vb> VbPtr;

typedef vector<String> OidVector;
typedef vector<String> ValueVector;
typedef vector<VbPtr> VbVector;
typedef Table<VbPtr> SnmpResultSet;

class Snmp : public noncopyable
{
public:
	Snmp()
		: _sessionPtr(NULL)
	{
		SOCK_STARTUP;
		//LOG_DEBUG<<"snmp init";
	}
	
	~Snmp()
	{
		close();
		SOCK_CLEANUP;
		//LOG_DEBUG<<"snmp exit";
	}
	
	bool open1(const string &ipAddr,
				   const string &community,
				   int port = 161,
				   int retries = 0,
				   int timeout = 3 * 1000 * 1000)
	{
		//LOG_DEBUG<<"snmp open1:"<<ipAddr<<':'<<port<<" community:"<<community;
		return open(ipAddr, community, port, retries, timeout, SNMP_VERSION_1);
	}
		
	bool open2(const string &ipAddr,
				   const string &community,
				   int port = 161,
				   int retries = 0,
				   int timeout = 3 * 1000 * 1000)
	{ 
		//LOG_DEBUG<<"snmp open2:"<<ipAddr<<':'<<port<<" community:"<<community;
		return open(ipAddr, community, port, retries, timeout, SNMP_VERSION_2c);
	}

	void close()
	{
		if (NULL != _sessionPtr) {
			snmp_close(_sessionPtr);
			_sessionPtr = NULL;
			//LOG_DEBUG<<"snmp close:"<<ipAddr<<':'<<port<<" community:"<<community;
		}
	}

	bool get(const string &oid, string &value)
	{
		OidVector oids;
		ValueVector values;
		oids.push_back(oid);
		if (get(oids, values) && values.size() > 0) {
			value = values[0];
			return true;
		}
		return false;
	}

	bool get(const OidVector &oids, ValueVector &values)
	{
		if (0 == oids.size()) {
			_errorStr = "no oids";
			//LOG_ERROR<<_errorStr;	
			return false;
		}
			
		netsnmp_pdu *pdu = NULL;
		netsnmp_pdu *response = NULL;
		netsnmp_variable_list *vars = NULL;
		
		if (NULL == (pdu = snmp_pdu_create(SNMP_MSG_GET))) {
			_errorStr = "snmpget create error";
			//LOG_ERROR<<_errorStr;	
			return false;
		}
		
		oid anOID[MAX_OID_LEN];
		size_t anOID_len = MAX_OID_LEN;
		bool bSuccess = false;
		int failures = 0;
		for (OidVector::iterator it = oids.begin(); oids.end() != itr; ++itr) {
			if (!snmp_parse_oid(itr->c_str(), anOID, &anOID_len)
					|| !snmp_add_null_var(pdu, anOID, anOID_len))
				++failures;
		}
			
		if (failures) {
			_errorStr = "snmpget add info error";
			//LOG_ERROR<<_errorStr;	
			return false;
		}
		
		int status = snmp_synch_response(_sessionPtr, pdu, &response);
		
		if (STAT_SUCCESS == status) {
			if (SNMP_ERR_NOERROR == response->errstat) {
				values.clear();
				for (vars = response->variables; vars; vars = vars->next_variable) {
					values.push_back(asn2String(vars));
					//LOG_DEBUG<<"snmpget success:"<<oid2String(vars->name, vars->name_length)<<' '<<asn2String(vars);
				}
				bSuccess = true;
			} else {// PDU错误
				//_errorStr = "snmpget PDU error";
				_errorStr = snmp_errstring(response->errstat);
				if (vars = response->variables) {
					_errorStr += ":";
					_errorStr += oid2String(vars->name, vars->name_length);
				}		
				//LOG_ERROR<<_errorStr;	
			}
		} else if (STAT_TIMEOUT == status) {// 超时
			_errorStr = "snmpget timeout";
			//LOG_ERROR<<_errorStr;	
		} else {// status == STAT_ERROR
			_errorStr = "snmpget other error";
			//LOG_ERROR<<_errorStr;	
		}
		
		if (response)
			snmp_free_pdu(response);
		
		return bSuccess;
	}
	
	bool getNext(string &oid, string &value)
	{
		OidVector oids;
		ValueVector values;
		oids.push_back(oid);
		if (getNext(oids, values) && oids.size() > 0 && values.size() > 0) {
			oid = oids[0];
			value = values[0];
			return true;
		}
		return false;
	}
	
	//分次取，需要外部循环判断
	bool getNext(OidVector &oids, ValueVector &values)
	{
		if (0 == oids.size()) {
			_errorStr = "no oids";
			//LOG_ERROR<<_errorStr;
			return false;
		} else {
			values.clear();
		}

		OidVector oldOids = oids;
		bool bSuccess = false;
		netsnmp_pdu *pdu = NULL;
		netsnmp_pdu *response = NULL;
		netsnmp_variable_list *vars = NULL;
		
		if (NULL == (pdu = snmp_pdu_create(SNMP_MSG_GETNEXT))) {
			_errorStr = "snmpgetnext create error";
			//LOG_ERROR<<_errorStr;
			return false;
		}
		
		oid anOID[MAX_OID_LEN];
		size_t anOID_len = MAX_OID_LEN;
		int failures = 0;
		OidVector::iterator itr = oids.begin();
		for (; oids.end() != itr; ++itr) {
			if (!snmp_parse_oid(itr->c_str(), anOID, &anOID_len)
					|| !snmp_add_null_var(pdu, anOID, anOID_len))
				++failures;
		}
		
		if (failures) {
			_errorStr = "snmpgetnext add info error";
			//LOG_ERROR<<_errorStr;
			return false;
		}
		
		int status = snmp_synch_response(_sessionPtr, pdu, &response);
		
		if (STAT_SUCCESS == status) {
			if (SNMP_ERR_NOERROR == response->errstat) {
				int i = 0;
				for (vars = response->variables; vars; vars = vars->next_variable, ++i) {
					oids[i] = oid2String(vars->name, vars->name_length);
					values.push_back(asn2String(vars));
					//LOG_DEBUG<<"snmpget success:"<<oid[i]<<' '<<asn2String(vars);
				}
				bSuccess = true;
			} else {
				// PDU
				_errorStr = snmp_errstring(response->errstat);
				if (vars = response->variables)
				{
					_errorStr += ":";
					_errorStr += oid2String(vars->name, vars->name_length);
				}	
				//LOG_ERROR<<_errorStr;
			}
		}
		else if (STAT_TIMEOUT == status)
		{
			_errorStr = "snmpgetnext timeout";
			//LOG_ERROR<<_errorStr;
		}
		else // status == STAT_ERROR
		{
			_errorStr = "snmpgetnext other error";
			//LOG_ERROR<<_errorStr;
		}
		
		if (response)
		{
			snmp_free_pdu(response);
			response = NULL;
		}
		return bSuccess;
	}

	//一次性取完再返回
	bool getTable(OidVector &oids, SnmpResultSet &values)
	{
		if (0 == oids.size())
		{
			_errorStr = "snmpgettable no oids";
			//LOG_ERROR<<_errorStr;
			return false;
		}
		else
		{
			values.clear();
		}
		OidVector oldOids = oids;
		bool bSuccess = false;
		int countFinish = 0;//完成snmpgetnext数
		std::vector<bool> checkFinish(oids.size(), false);//完成snmpgetnext的标志
		
		while (countFinish < oids.size())
		{
			bSuccess = false;
			_errorStr = "no error";
			netsnmp_pdu *pdu = NULL;
			netsnmp_pdu *response = NULL;
			netsnmp_variable_list *vars = NULL;
			
			if (NULL == (pdu = snmp_pdu_create(SNMP_MSG_GETNEXT)))
			{
				_errorStr = "snmpgettable create error";
				//LOG_ERROR<<_errorStr;
				return false;
			}
			
			oid anOID[MAX_OID_LEN];
			size_t anOID_len = MAX_OID_LEN;
			int failures = 0;
			OidVector::iterator itr = oids.begin();
			for (; oids.end() != itr; ++itr)
			{
				if (!snmp_parse_oid(itr->c_str(), anOID, &anOID_len)
					|| !snmp_add_null_var(pdu, anOID, anOID_len))
				{
					++failures;
				}
			}
			
			if (failures)
			{
				_errorStr = "snmpgettable add info error";
				//LOG_ERROR<<_errorStr;
				return false;
			}
			
			int status = snmp_synch_response(_sessionPtr, pdu, &response);
			
			std::vector<VB_PTR>* row = NULL;
			
			if (STAT_SUCCESS == status)
			{
				if (SNMP_ERR_NOERROR == response->errstat)
				{
					int i = 0;
					bool bAddRow = true;//判断是否插入新一行
					
					for (vars = response->variables; vars; vars = vars->next_variable, ++i)
					{	
						oids[i] = oid2String(vars->name, vars->name_length);
						if (oids[i].substr(0, oldOids[i].length()) == oldOids[i])
						{
							if (bAddRow)
							{
								row =  values.addRow();
								bAddRow = false;
							}
							VB_PTR vb_ptr(new VB);
							vb_ptr->oid = oids[i];
							vb_ptr->type = vars->type;
							vb_ptr->value = asn2String(vars);
							(*row).push_back(vb_ptr);
						}
						else
						{
							checkFinish[i] = true;
						}
						//LOG_DEBUG<<"snmp getTable:"<<oids[i]<<' '<<asn2String(vars);
					}

					bSuccess = true;
				}
				else// PDU error
				{
					_errorStr = snmp_errstring(response->errstat);
					if (vars = response->variables)
					{
						_errorStr += ":";
						_errorStr += oid2String(vars->name, vars->name_length);
					}
					if (response)
					{
						snmp_free_pdu(response);
						response = NULL;
					}
					//LOG_ERROR<<_errorStr;
					break;
				}
			}
			else if (STAT_TIMEOUT == status)
			{
				_errorStr = "snmpgettable timeout";
				//LOG_ERROR<<_errorStr;
				break;
			}
			else // status == STAT_ERROR
			{
				_errorStr = "snmpgettable other error";
				//LOG_ERROR<<_errorStr;
				break;
			}
			
			if (response)
			{
				snmp_free_pdu(response);
				response = NULL;
			}
				
			countFinish = 0;
			for (int i = 0; i < checkFinish.size(); ++i)
			{
				if (checkFinish[i])
				{
					++countFinish;
				}
			}
		}
		return bSuccess;
	}

	bool set(String &inOid, char type, String &value)
	{
		SnmpSetInputArray input;
		VB_PTR ptr(new VB);
		ptr->oid = inOid;
		ptr->type = type;
		ptr->value = value;
		input.push_back(ptr);
		return set(input);
	}
	
	bool set(SnmpSetInputArray &input)
	{
		if (0 == input.size())
		{
			_errorStr = "snmpset no input";
			//LOG_ERROR<<_errorStr;
			return false;
		}
		//检查输入
		for (int i=0; i < input.size(); ++i)
		{
			if (NULL == input[i])
			{
				_errorStr = "snmpset input error";
				//LOG_ERROR<<_errorStr;
				return false;
			}
		}
			
		netsnmp_pdu *pdu = NULL;
		netsnmp_pdu *response = NULL;
		netsnmp_variable_list *vars = NULL;
		
		if (NULL == (pdu = snmp_pdu_create(SNMP_MSG_SET)))
		{
			_errorStr = "snmpset create error";
			//LOG_ERROR<<_errorStr;
			return false;
		}
		
		oid anOID[MAX_OID_LEN];
		int failures = 0;
		bool bSuccess = false;
		
		for (int i=0; i < input.size(); ++i)
		{
			size_t anOID_len = MAX_OID_LEN;
			if (NULL == snmp_parse_oid(input[i]->oid.c_str(), anOID, &anOID_len)) 
			{
				++failures;
			} 
			else if (snmp_add_var(pdu, anOID, anOID_len, input[i]->type, input[i]->value.c_str())) //success return 0
			{
				++failures;
			}
		}
		
		if (failures)
		{
			_errorStr = "snmpset add info error";
			//LOG_ERROR<<_errorStr;
			return false;
		}
			
		int status = snmp_synch_response(_sessionPtr, pdu, &response);
		
		if (STAT_SUCCESS == status)
		{
			if (SNMP_ERR_NOERROR == response->errstat)
			{
				for (vars = response->variables; vars; vars = vars->next_variable)
				{
					//LOG_DEBUG<<"snmpset:"<<oid2String(vars->name, vars->name_length)<<" "<<asn2String(vars);
				}
				bSuccess = true;
			}
			else// PDU错误
			{
				//_errorStr = "snmpget PDU error";
				_errorStr = snmp_errstring(response->errstat);
				if (vars = response->variables)
				{
					_errorStr += ":";
					_errorStr += oid2String(vars->name, vars->name_length);
				}	
				//LOG_ERROR<<_errorStr;
			}
		}
		else if (STAT_TIMEOUT == status)// 超时
		{
			_errorStr = "snmpset timeout";
			//LOG_ERROR<<_errorStr;
		}
		else // status == STAT_ERROR
		{
			_errorStr = "snmpset other error";
			//LOG_ERROR<<_errorStr;
		}
		
		if (response)
			snmp_free_pdu(response);
		
		return bSuccess;
	}
	
	//逗号分隔的字符串DateAndTime转为"2014:4:22,12:12:12.0,-7:0"格式
	// String convert2Datetime(String& str)
	// {
		// if (',' == str[str.length() - 1])
		// {
			// str[str.length() - 1] = '\0';
		// }
		// char buf[MAX_OID_LEN] = {0};
		// int i1 = 0, i2 = 0, i3 = 0, i4 = 0, i5 = 0, i6 = 0,
		// i7 = 0, i8 = 0, i9 = 0, i10 = 0, i11 = 0;
		// sscanf(str.c_str(), "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", 
			// &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &i9, &i10, &i11);
		// sprintf(buf, "%2d:%1d:%1d,%1d:%1d:%1d.%1d,%c%1d:%1d", 
			// i1*256 + i2, i3, i4, i5, i6, i7, i8, i9, i10, i11);
		// return buf;
	// }
	
private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(Snmp);

	bool open(const string &ipAddr, const string &community,
			  int port, int retries, int timeout, int version)
	{
		char addr[128];
		snprintf(addr, sizeof(addr), "%s:%d", ipAddr.c_str(), port);
		
		char commun[128];
        snprintf(commun, sizeof(commun),"%s", community.c_str());

		snmp_sess_init(&_session);
		_session.version = version; // 
		_session.retries = retries; //
		_session.timeout =  timeout; //
		_session.peername = addr;
		_session.remote_port = port; //
		_session.community = (u_char *)commun;
		_session.community_len = strlen(commun);
		
		if (NULL == (_sessionPtr = snmp_open(&_session)))
			return false;

		return true;
	}
	
    static string oid2String(const oid *oid, int len)
    {
        char str[MAX_OID_LEN] = {0};
        char tmp[128] = {0};
        for (int i = 0; i < len; ++i) {
            snprintf(tmp, sizeof(tmp), ".%ld", oid[i]);
            strcat(str, tmp);
        }
        return str;
    }
	
	static string asn2String(const netsnmp_variable_list *var)
	{
		char buf[MAX_OID_LEN] = {0};
		
		switch (var->type) {			
		case ASN_INTEGER:
			snprintf(buf, MAX_OID_LEN,"%ld", *var->val.integer);
			break;
		case ASN_BIT_STR:
		case ASN_OCTET_STR:
		{
			String str;
			if (var->val_len > 0 && var->val_len <= 40 && '\0' == var->val.string[0])
			{//组装MAC
				for (int i = 0; i < var->val_len; ++i)
				{
					snprintf(buf, MAX_OID_LEN,"%02X:", var->buf[i]);
					str += buf;
				}
				str[str.length() - 1] = '\0';
				return str;
			}
			// else if (!checkIsPrint(var->val.string, var->val_len))
			// {
				// for (int i = 0; i < var->val_len; ++i)
				// {
					// snprintf(buf, MAX_OID_LEN, "%d", var->val.string[i]);						
					// str += buf;
					// str += ',';
				// }
				// if (!str.empty())
				// {
					// str[str.length() - 1] = '\0';
				// }
			// }
			else
			{
				for (int i = 0; i < var->val_len; ++i)
				{
					str += var->val.string[i];
				}
			}

			return str;
		}
		
		case ASN_OBJECT_ID:
		{
			return oid2String(var->name, var->name_length);
		}
		case ASN_IPADDRESS:
		{
			struct in_addr addr = {0};
			memcpy(&addr, var->val.objid, var->val_len);
			return inet_ntoa(addr);
		}
		
		case ASN_OPAQUE_FLOAT:
        {
			snprintf(buf, MAX_OID_LEN, "%f", *var->val.floatVal);
			return buf;
        }
		
		case ASN_OPAQUE_DOUBLE:
        {
			snprintf(buf, MAX_OID_LEN, "%lf", *var->val.doubleVal);
			return buf;
        }
		
		case ASN_NULL:
            strcpy "null";
		
		case ASN_COUNTER64:
		case ASN_OPAQUE_U64:
		case ASN_OPAQUE_I64:
		case ASN_OPAQUE_COUNTER64:
		case ASN_OPAQUE:	
		case ASN_GAUGE:
		case ASN_COUNTER:
		case ASN_UINTEGER:
		case ASN_TIMETICKS:
			snprintf(buf, MAX_OID_LEN, "%lu", /*var->val.counter64->low<<32 + */var->val.counter64->high);//32位系统不取不到64位的值
			break;
		default:
			//LOG_ERROR<<"other type return 0";
			strcpy(buf, "");
			break;
		}
		return buf;
	}
	
	struct snmp_session _session;
	struct snmp_session *_sessionPtr;
};

} // nameapace thefox

#endif // _THEFOX_SNMP_H_


#ifndef _THEFOX_SNMP_OID_H_
#define _THEFOX_SNMP_OID_H_

namespace thefox
{
namespace snmp
{
class Oid
{
public:
	Oid(const char *oidStr)
	{
		setOid(oidStr);
	}
	void setOid(oid
	
private:
	typedef struct
	{
		unsigned int value;
		struct Oid *pre
		struct Oid *next;
	} oid_t;
	
	oid_t *_head;
	oid_t *_tail;
};

} // namespace thefox
} // namespace snmp
#endif // _THEFOX_SNMPOID_H_
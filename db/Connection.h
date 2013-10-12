

class IConnection
{
public:
	virtual bool open(...) = ;
	virtual void close() = 0;
	virtual bool query() = 0;
	

};
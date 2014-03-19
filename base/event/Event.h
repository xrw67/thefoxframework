

class Event
{
public:
#ifdef WIN32
	OVERLAPPED _ol;
#endif
	
private:
	int32_t _type;
}
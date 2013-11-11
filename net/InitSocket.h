class InitSocket
{
public:
	InitSocket(LPWSADATA lpWSAData = nullptr, BYTE minorVersion = 2, BYTE majorVersion = 2)
	{
		LPWSADATA temp = lpWSAData;
		if(!temp)
			temp = (LPWSADATA)_alloca(sizeof(WSADATA));

		_result = ::WSAStartup(MAKEWORD(minorVersion, majorVersion), temp);
	}

	~InitSocket()
	{
		if (IsValid())
			::WSACleanup();
	}

	int	GetResult() {return m_iResult;}
	BOOL IsValid() {return m_iResult == 0;}

private:
	int _result;
};
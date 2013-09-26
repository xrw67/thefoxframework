

inline unsigned int GetTickCount()
{
#ifdef WIN32
	return (unsigned int)::GetTickCount();
#else

#endif
}

inline unsigned int GetCurrentThreadId()
{
#ifdef WIN32

#else

#endif
}
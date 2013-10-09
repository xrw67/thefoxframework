#ifndef _TF_THREAD_H_
#define _TF_THREAD_H_

#include "afxmt.h"

#ifdef USE_NAMESPACE
namespace TheFox {
#endif

#ifdef __cpluscpus
extern "C" {
#endif

class TFThread
{
public:
	TFThread()
	{
	}
	~TFThread()
	{
	}
	
	void SetParam(LPVOID p) { m_param = p; }
	
	LPVOID GetParam() { return m_param; }
	
	CWinThread *GetThread() { return m_pThread; }
	
	void Run()
	{
		m_workstatus = true;
		m_pThread = AfxBeginThread(sThreadFunc, this);
	}
	
	void Stop()
	{
		m_workstatus = false;
		::WaitForSingleObject(m_endEvent, INFINITE);
	}
protected:
	bool GetWorkStatus() { return m_workstatus; }
	
	void SetStopFlag() { m_endEvent.SetEvent(); }
	
	virtual UINT ThreadFunc() = 0;
private:
	static UINT sThreadFunc(LPVOID p)
	{
		UINT retCode = 0;
		TFThread *pMain = (TFThread *)p;
		retCode = pMain->ThreadFunc();
		pMain->SetStopFlag();
		return retCode;
	}

	LPVOID m_param;
	CWinThread *m_pThread;
	bool m_workstatus;
	CEvent m_endEvent;
};

#ifdef __cpluscpus
};
#endif

#ifdef USE_NAMESPACE
};
#endif

#endif // _TF_THREAD_H_
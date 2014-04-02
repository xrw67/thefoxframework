#ifndef _THEFOX_BASE_MAILSLOT_H_
#define _THEFOX_BASE_MAILSLOT_H_

#include <base/noncopyable.h>

namespace thefox {
    
DROWD WINAPI mailslotThreadProc(LPVOID lpParameter);
    
class MailsoltServer : noncopyable
{
public:
    MailsoltServer()
        : _bCreated(false)
    {}
    ~CMailsoltServer() 
    {}
    
    bool create(LPCTSTR lpszSlotName)
    {
        _hSlot = CreateMailslot(lpszSlotName, 0, MAILSLOT_WAIT_FOREVER, (LPSECURITY_ATTRIBUTES)NULL);
        
        if (_hSlot == INVALID_HANDLE_VALUE) {
            //TRACE("CreateMailslot failed with %d\n", GetLastError());
            return false;
        }
        
        _hReadThread= ::CreateThread(NULL, 0, mailslotThreadProc, this, 0, NULL);
    }
    
    void close()
    {
        TerminateThread(_hReadThread, 4);
        CloseHandle(_hReadThread);
        CloseHandle(_hSlot);
    }
    
    virtual void onRead(const DWORD dataLen) = 0;
    
private:
    bool _bCreated;
    HANDLE _hSlot;
    HANDLE _hReadThread;
};
    
    
class CMailslotClient
{
public:
    CMailslotClient()
    {}
    ~CMailslotClient()
    {}
    
    bool open(LPCTSTR SlotName)
    {
        m_hFile = CreateFile(SlotName, GENERIC_WRITE, FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES) NULL,
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
        if (m_hFile == INVALID_HANDLE_VALUE) {
            //TRACE("CreateFile failed with %d.\n", GetLastError());
            return false;
        }
        return true;
    }
    
    void close()
    {
        ::CloseHandle(m_hFile);
    }
    
    bool write(const char *data, const size_t dataLen)
    {
        BOOL fResult;
        DWORD cbWritten;
        
        fResult = WriteFile(m_hFile, data, (DWORD)dataLen, &cbWritten, (LPOVERLAPPED)NULL);
        if (!fResult) {
            //TRACE("WriteFile failed with %d.\n", GetLastError());
            return false; 
        } 
        
        return true;
    }
    
private:
    HANDLE _hFile;
}


DROWD WINAPI MailslotThreadProc(LPVOID lpParameter)
{
    CMailslotServer *svr = (CMailslotServer *)param;
    BOOL fResult; 
    
    while (1) {
        if (!svr->m_bCreated)
            return 0;
        
        DWORD cbMessage, cMessage; 
        cbMessage = cMessage = 0; 
        
        cbMessage = cMessage = cbRead = 0; 
        GetMailslotInfo(svr->m_hSlot, (LPDWORD)NULL, &cbMessage, &cMessage, (LPDWORD)NULL);
        
        if (cMessage > 0)
            svr->OnRead(cMessage);
    }
    return 0;
}
    
} // namespace thefox

#endif // _THEFOX_BASE_MAILSLOT_H_
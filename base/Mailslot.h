#ifndef _THEFOX_MAILSLOT_H_
#define _THEFOX_MAILSLOT_H_

#include <base/noncopyable.h>

namespace thefox {
    
DROWD WINAPI MailslotThreadProc(LPVOID lpParameter);
    
class MailsoltServer : noncopyable
{
public:
    MailsoltServer()
    :
        {
            m_bCreated = false;
        }
        ~CMailsoltServer() {};
        bool Create(LPCTSTR lpszSlotName)
        {
            m_hSlot = CreateMailslot(lpszSlotName, 0, MAILSLOT_WAIT_FOREVER, (LPSECURITY_ATTRIBUTES) NULL);
            
            if (m_hSlot == INVALID_HANDLE_VALUE)
            {
                //TRACE("CreateMailslot failed with %d\n", GetLastError());
                return false;
            }
            
            m_hReadThread= ::CreateThread(NULL, 0, MailslotThreadProc, this, 0, NULL);
        }
        
        void Close()
        {
            TerminateThread(m_hReadThread, 4);
            CloseHandle(m_hReadThread);
            CloseHandle(m_hSlot);
        }
        
        virtual void OnRead(const DWORD dataLen) = 0;
        
    private:
        bool m_bCreated;
        HANDLE m_hSlot;
        HANDLE m_hReadThread;
    };
    
    
    class CMailslotClient
    {
    public:
        CMailslotClient();
        ~CMailslotClient();
        bool Open(LPCTSTR SlotName)
        {
            m_hFile = CreateFile(SlotName, GENERIC_WRITE, FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES) NULL,
                                 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
            if (m_hFile == INVALID_HANDLE_VALUE)
            {
                //TRACE("CreateFile failed with %d.\n", GetLastError());
                return false;
            }
            return true;
        }
        
        void Close()
        {
            ::CloseHandle(m_hFile);
        }
        
        bool Write(const char *data, const size_t dataLen)
        {
            BOOL fResult;
            DWORD cbWritten;
            
            fResult = WriteFile(m_hFile, data, (DWORD)dataLen, &cbWritten, (LPOVERLAPPED)NULL);
            if (!fResult)
            {
                //TRACE("WriteFile failed with %d.\n", GetLastError());
                return false; 
            } 
            
            return true;
        }
        
    private:
        HANDLE m_hFile;
    }
    
    
    DROWD WINAPI MailslotThreadProc(LPVOID lpParameter);
    {
        CMailslotServer *svr = (CMailslotServer *)param;
        BOOL fResult; 
        
        while (1)
        {
            if (!svr->m_bCreated)
            {
                return 0;
            }
            
            DWORD cbMessage, cMessage; 
            cbMessage = cMessage = 0; 
            
            cbMessage = cMessage = cbRead = 0; 
            GetMailslotInfo(svr->m_hSlot, (LPDWORD)NULL, &cbMessage, &cMessage, (LPDWORD)NULL);
            
            if (cMessage > 0)
            {
                svr->OnRead(cMessage);
            }
        }
        return 0;
    }
    
} // namespace thefox
#endif // _THEFOX_MAILSLOT_H_
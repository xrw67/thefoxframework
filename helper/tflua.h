#ifndef _TF_LUA_H_
#define _TF_LUA_H_

#include "lua.h"
#include "lualib.h"
#include "luaxlib.h"
#include "tolua++"

class TFLua
{
public:
    TFLua()
    {
    
    }
    ~TFLua()
    {
    
    }
    
    void Init();
    {
        if (NULL == m_state)
        {
            m_state = lua_open();
            luaL_Openlibs(m_state);
        }
    }
    void Close()
    {
        if (NULL != m_state)
        {
            lua_close(m_state);
            m_state = NULL;
        }
    }
    
    bool LoadLuaFile(const char *filename);
    {
        int retCode = 0;
        if (NULL == m_state)
        {
            return false;
        }
        
        retCode = luaL_dofile(m_state, filename);
        if (0 !=retCode)
        {
            return false;
        }
        return true;
    }

private:
    lua_State *m_state;

};

#endif
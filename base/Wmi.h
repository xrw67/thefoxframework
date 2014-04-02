#ifndef _THEFOX_BASE_WMI_H_
#define _THEFOX_BASE_WMI_H_

#include <base/copyable.h>

namespace thefox
{

class WmiRowset : copyable
{
public:
    typedef std::map<String, String> RowType;
    
    String get(String &columnName) {
        RowType::iterator it;
        if ((it = _row.find(columnName)) != std::map::npos)
            return *it;
        return "NULL";
    }
    
    const int columnCount() const { return m_row.size(); }
    const int rowIndex() const { return _rowIndex; }
    
private:
    RowType _row;
    int _rowIndex;
};

class WmiResultset
{
public:
    WmiResultset(IEnumWbemClassObject enumerator);
    bool getFirst(WmiRowset &row)
    { 
    
    }
    
    bool getNext(WmiRowset &row)
    {
        
    }
    
private:
    std::vector<WmiRowset> _resset;
};

class Wmi
{

};

}

#endif // _THEFOX_BASE_WMI_H_
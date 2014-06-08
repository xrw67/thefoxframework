/*
* @filename MySqlResultSet.h
* @brief libmysql结果集的C++封装类，支持Windows和Linux
* @author macwe1024 at gmail dot com
*/

#ifndef _THEFOX_DB_MYSQL_RESULTSET_H_
#define _THEFOX_DB_MYSQL_RESULTSET_H_

#include <mysql/mysql.h>
#include <base/Types.h>
#include <base/noncopyable.h>

namespace thefox
{

namespace db
{

/// @beirf 结果集类
class MySqlResultSet :noncopyable
{
public:
    MySqlResultSet()
        : _res(NULL)
        , _row(NULL)
    {}
    
    ~MySqlResultSet()
    {
        if (_res) {
            mysql_free_result(_res);
            _res = NULL;
        }
    }
    
    /// @beirf 得到字段的数量
    int fieldCount() const
    {
        if (!_res)
            return 0;
        return _res->field_count;
    }
    
    /// @beirf 得到索引指定的字段的名称
    const char *fieldName(int index) const
    {
        if (!_res)
            return "";
        if (index < 0 || index >= _res->field_count)
            return "";
        return _res->fields[index].name;
    }
    
    /// @beirf 得到结果集中有多少行记录
    int rowCount() const
    {
        if (!_res)
            return 0;
        return (int)_res->row_count;
    }
    
    /// @beirf 取下一行的记录
    /// @return true表示成功取到下一行，false表示已经全部取完了
    bool fetchRow()
    {
        if (!_res)
            return false;
        if (_row = mysql_fetch_row(_res))
            return true;
        else
            return false;
    }
    
    /// @beirf 根据索引得到字符串类型的值
    /// @param[in] index 字段索引号
    /// @return 返回对应的值
    const char *getString(const int index) const 
    {
        if (!_res || !_row)
            return "";
        if (index < 0 || index >= rowCount())
            return "";
        return _row[index] ? _row[index] : "";
    }
    
    /// @beirf 根据索引得到字符串类型的值
    /// @param[in] fiendName 字段的名称
    /// @return 返回对应的值
    const char *getString(const String &fieldName) const
    {
        int index = getFieldIndex(fieldName);
        if (index >= 0)
            return _row[index] ? _row[index] : "";
        return "";
    }
    
    /// @beirf 根据索引得到int类型的值
    /// @param[in] index 字段索引号
    /// @return 返回对应的值
    int getInt(const int index) const
    { return atoi(getString(index)); }
    
    /// @beirf 根据索引得到int类型的值
    /// @param[in] fiendName 字段的名称
    /// @return 返回对应的值
    int getInt(const string &fieldName) const
    { return atoi(getString(fieldName)); }
    
    /// @beirf 根据索引得到uint类型的值
    /// @param[in] index 字段索引号
    /// @return 返回对应的值
    uint32_t getUInt(const int index) const
    { return (uint32_t)atoi(getString(index)); }
    
    /// @beirf 根据索引得到uint类型的值
    /// @param[in] fiendName 字段的名称
    /// @return 返回对应的值
    uint32_t getUInt(const string &fieldName) const
    { return (uint32_t)atoi(getString(fieldName)); }
    
    /// @beirf 根据索引得到一个字符串类型值
    /// @return 返回索引号对应的值
    /// @sa getString
    const char *operator[](const int index) const
    {
        return getString(index);
    }
    
    operator bool() const 
    { return NULL == _res; }
    
    operator MYSQL_RES *() const
    { return _res; }
    
    const MySqlResultSet &operator=(MYSQL_RES *res)
    {
        _res = res;
        return *this;
    }

private:
    int getFieldIndex(const string &fieldName) const
    {
        for (int i = 0; i < _res->field_count; ++i) {
        if (fieldName == _res->fields[i].name)
            return i;
        }
        return -1;
    }
    
    MYSQL_RES *_res;
    MYSQL_ROW _row;
};

} // namespace db

} // namespace thefox

#endif // _THEFOX_DB_MYSQL_RESULTSET_H_

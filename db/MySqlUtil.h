/*
* @filename MySqlUtil.h
* @brief mysql的一些好用的辅助函数，支持Windows和Linux
* @author macwe@qq.com
*/

#ifndef _THEFOX_DB_MYSQL_UTIL_H_
#define _THEFOX_DB_MYSQL_UTIL_H_

#include <base/Types.h>
#include <db/MySqlConnection.h>

namespace thefox
{

namespace db

class MysqlUtil
{
public:
    static bool isTableExist(MySqlConnection &conn, const string &tableName)
    {
        MySqlResultSet resultSet;
        string sql("SHOW TABLES LIKE '"+ tableName+"'");
        conn.query(sql, resultSet);
        if (0 == resultSet.rowCount())
            return false;
        else
            return true;
    }

    static bool isQueryNotRecord(MySqlConnection &conn, const string &selectSql)
    {
        MySqlResultSet resultSet;
        conn.query(selectSql, resultSet);
        if (0 == resultSet.rowCount())
            return false;
        else
            return true;
    }

    static bool setNames(const string &csName)
    {
        string sql("SET NAMES '"+ csName+"'");
        return conn.exec(sql);
    }

};

} // nsmespace db

} // namespace thefox

#endif // _THEFOX_DB_MYSQL_UTIL_H_


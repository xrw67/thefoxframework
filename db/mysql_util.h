/**
* @filename mysql_util.h
* @brief mysql的一些好用的辅助函数，支持Windows和Linux
* @author macwe1024 at gmail dot com
*/

#ifndef _THEFOX_DB_MYSQL_UTIL_H_
#define _THEFOX_DB_MYSQL_UTIL_H_

#include <map>
#include <base/common.h>
#include <db/mysql_connection.h>

namespace thefox {
namespace db {

class MysqlUtil
{
public:
	typedef std::map<string, string> VariableMap;
	typedef std::map<string, string> StatusMap;
	
    static bool isTableExist(MySqlConnection &conn, const string &tableName)
    {
        MySqlResultSet resultSet;
        string sql("SHOW TABLES LIKE '"+ tableName+"'");
        conn.query(sql, resultSet);
        if (0 == resultSet.rowCount()) {
            return false;
        } else {
            return true;
		}
    }

    static bool isQueryNotRecord(MySqlConnection &conn, const string &selectSql)
    {
        MySqlResultSet resultSet;
        conn.query(selectSql, resultSet);
        if (0 == resultSet.rowCount()) {
            return false;
        } else {
            return true;
		}
    }

    static bool setNames(MySqlConnection &conn, const string &csName)
    {
        string sql("SET NAMES '"+ csName+"'");
        return conn.exec(sql);
    }

	static VariableMap getVariable(MySqlConnection &conn, const string &keys)
	{
		VariableMap result;
		MySqlResultSet rs;
		string strsql("show variables");
		if (!keys.empty()) {
			strsql += " where Variable_name in (";
			strsql += keys;
			strsql += ")";
		}
		
		conn.query(strsql, rs);
		while (rs.fetchRow()) {
			result[rs.getString(0)] = rs.getString(1);
		}
		
		return result;
	}
	
	static StatusMap getGlobalStatus(MySqlConnection &conn, const string &keys)
	{
		StatusMap result;
		MySqlResultSet rs;
		string strsql("show global status");
		if (!keys.empty()) {
			strsql += " where Variable_name in (";
			strsql += keys;
			strsql += ")";
		}
		
		conn.query(strsql, rs);
		while (rs.fetchRow()) {
			result[rs.getString(0)] = rs.getString(1);
		}
		
		return result;
	}
};

} // nsmespace db
} // namespace thefox

#endif // _THEFOX_DB_MYSQL_UTIL_H_


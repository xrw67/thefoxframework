/*
* @filename MySqlUtil.h
* @brief mysql的一些好用的辅助函数，支持Windows和Linux
* @author macwe@qq.com
*/

#ifndef _THEFOX_MYSQL_UTIL_H_
#define _THEFOX_MYSQL_UTIL_H_

#include <base/Types.h>
#include "MySqlConnection"

namespace thefox
{

namespace mysql
{

bool isTableExist(MySqlConnection &conn, const String &tableName)
{
	MySqlResultSet resultSet;
	String sql("SHOW TABLES LIKE '"+ tableName+"'");
	conn.query(sql, resultSet);
	if (0 == resultSet.rowCount())
		return false;
	else
		return true;
}

bool isQueryNotRecord(MySqlConnection &conn, const String &selectSql)
{
	MySqlResultSet resultSet;
	conn.query(selectSql, resultSet);
	if (0 == resultSet.rowCount())
		return false;
	else
		return true;
}

bool setNames(const String &csName)
{
	String sql("SET NAMES '"+ csName+"'");
	return conn.exec(sql);
}

}; // namespace thefox

}; // namespace thefox

#endif // _THEFOX_MYSQL_UTIL_H_
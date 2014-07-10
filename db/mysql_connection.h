/*
* @filename MySqlConnection.h
* @brief libmysql连接管理C++封装类，支持Windows和Linux
* @author macwe1024 at gmail dot com
*/

#ifndef _THEFOX_DB_MYSQL_CONNECTION_H_
#define _THEFOX_DB_MYSQL_CONNECTION_H_

#include <base/mutex.h>
#include <db/mysql_result_set.h>

namespace thefox {
namespace db {

/// @beirf mysql数据库连接类
class MySqlConnection
{
public:
    MySqlConnection(const string &host = "",
                    const string &user = "",
                    const string &passwd = "",
                    const int port = 3306,
                    const string &dbName = "")
        : _connPtr(NULL)
        , _host(host)
        , _user(user)
        , _passwd(passwd)
        , _port(port)
        , _dbName(dbName)
    {}

    ~MySqlConnection()
	{ 
		this->close(); 
	}

    MySqlConnection(const MySqlConnection &db)
		: _connPtr(NULL)
	{
		setParam(db._host, db._user, db._passwd, db._port, db._dbName);
	}

    /// @brief 设置数据库连接参数
    /// @param[in] host 主机名或ip地址
    /// @param[in] user 用户名
    /// @param[in] passwd 密码
    /// @param[in] port 主机端口号
    /// @param[in] dbName 数据库名
    void setParam(const string &host, const string &user, const string &passwd, int port, const string &dbName)
    {
        _host = host;
        _user = user;
        _passwd = passwd;
        _port = port;
        _dbName = dbName;
    }
    
    /// @brief 连接数据库
    /// @return 连接成功返回true，连接失败返回false
    bool open()
    { 
		this->close();
		_connPtr = &_conn;        
        mysql_init(_connPtr);

		unsigned int timeout = 3;
		mysql_options(_connPtr, MYSQL_OPT_CONNECT_TIMEOUT, (char*)&(timeout));//设置连接超时
       
		if (!mysql_real_connect(_connPtr, _host.c_str(), _user.c_str(), _passwd.c_str(), 
				0, _port, NULL, CLIENT_MULTI_QUERIES)) {
			this->close();
			return false;
		}

		if (!_dbName.empty()) {
			mysql_select_db(_connPtr, _dbName.c_str());
		}
		return true;
    }
    
    /// @beirf 关闭数据库连接
    void close()
    {
        if (_connPtr) {
            mysql_close(_connPtr);
        }

		_connPtr = NULL;
    }
    
	bool reopen()
	{
		if(_connPtr) {
			mysql_close(_connPtr);
		}
		
		if (!mysql_real_connect(_connPtr, _host.c_str(), _user.c_str(), _passwd.c_str(), 
				0, _port, NULL, CLIENT_MULTI_QUERIES)) {
			return false;
		}

		if (!_dbName.empty()) {
			mysql_select_db(_connPtr, _dbName.c_str());
		}

		return true;
	}
    
	bool ping()
	{
		if(!_connPtr) {
			return false;
		}

		return (0 == mysql_ping(_connPtr));
	}

    /// @brief 选择数据库
    /// @return 设置成功返回true，否则返回false
    bool selectDb(const string &dbName) 
    { 
        _dbName = dbName; 
        return (0 != mysql_select_db(_connPtr, _dbName.c_str()));
    }
    
    /// @brief 执行无返回结果集的查询语句
    /// @param[in] sql 待执行的sql语句
    /// @param[in|out] insertId 输出操作记录的ID
    ///    (注：对于INSERT和UPDATE语句且设置AUTO_INCREMENT才有效)
    /// @return 执行成功返回true, 否则返回false
    bool exec(const string &sql, uint32_t *insertId = NULL)
    {
        if (NULL == _connPtr) {
            return false;
        }
        
        MutexGuard lock(_lock);
        if (0 != mysql_real_query(_connPtr, sql.c_str(), (unsigned long)sql.length())) {
            return false;
        }

        if (insertId) {
            *insertId = (uint32_t)mysql_insert_id(_connPtr);
		}
        return true;
    }
    
    /// @brief 执行SELECT语句
    /// @param[in] sql 待执行的sql语句
    /// @param[in|out] resultSet 返回的结果集
    /// @return 执行成功返回true, 否则返回false
    /// @sa resultSet
    bool query(const string &sql, MySqlResultSet &resultSet)
    {
        if (NULL == _connPtr) {
            return false;
        }
        
        MutexGuard lock(_lock);
        if (0 != mysql_real_query(_connPtr, sql.c_str(), sql.length())) {
            return false;
        }
        
        if (resultSet) {
            mysql_free_result(resultSet);
		}

        resultSet = mysql_store_result(_connPtr);

        return true;
    }
        
private:
	const MySqlConnection &operator=(const MySqlConnection &);

    MYSQL _conn;
    MYSQL *_connPtr;
    Mutex _lock;

    string _host;
    string _user;
    string _passwd;
    string _dbName;
    int32_t _port;
};

} // namespace db

} // namesapce thefox

#endif // _THEFOX_DB_MYSQL_CONNECTION_H_

# 对libmysql简单的C++封装

**使用方法：**

    #include <database/MySqlConnection.h>
    using namespace thefox::db;
    
    int main(int argc, char **argv)
    {
    	
    	
    	MySqlConnection db("localhost", "root", "srdLHnms");
    	db.open();
    	MySqlResultSet rs;
    	db.query("select id,name from table", rs))
    	while (rs.fetchRow())
    	{
    		int id = rs.getInt(0);
    		String name = rs.getString(1);
    	}
    	db.close();
    	
    	return 0;
    }
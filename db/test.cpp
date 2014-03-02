#include <stdio.h>
#include <db/MySqlConnection.h>

using namespace mysql.db;

int main(int argc, char *argv[])
{
    MySqlConnection mysql;
    mysql.setParam("localhost","root","root",3306,"test");
    mysql.open();
    MySqlResultSet rs;
    if (mysql.query("select now()", rs))
    {
        while (rs.FetchRow())
        {
            printf("%s\r\n", rs.getString(0));
        }
    }
    return 0;
}

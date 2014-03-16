#include <WinSock2.h>
#include <base/Types.h>
#include <database/MySqlConnection.h>

int main(int argc, char *argv[])
{
	WSADATA wsa={0};
    WSAStartup(MAKEWORD(2,2),&wsa);

	thefox::mysql::MySqlConnection mysql("localhost", "root", "srdLHnms", 3306, "netmanager");
	bool retCode = mysql.open();

	thefox::mysql::MySqlResultSet resultSet;
	retCode = mysql.query("select * from switcher_type", resultSet);
	int rowcount = resultSet.rowCount();
	int fieldcount = resultSet.fieldCount();
	thefox::String fn = resultSet.fieldName(2);
	thefox::String buf;
	while (resultSet.fetchRow())
	{
		int dd = resultSet.getUInt(0);
		buf += resultSet[0];
		buf += resultSet[1];
		buf += resultSet[2];
		buf += resultSet[3];
		buf += "\n";
		//buf += resultSet.getString(0);
	}

	mysql.close();

	WSACleanup();

	return EXIT_SUCCESS;
}
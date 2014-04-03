#include <stdio.h>
#include <base/Timestamp.h>

int main(int argc, char *argv[])
{
	printf("%ld\n", time(NULL));
	thefox::Timestamp t(thefox::Timestamp::now());
	printf("%s\n", t.toString().c_str());
	printf("%s\n", t.toFormatString().c_str());
	return 0;
}

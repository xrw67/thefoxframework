#include <stdio.h>

#include <base/atomic_integer.h>

int main(int argc, char *argv[])
{
	thefox::AtomicInt32 i32;
	i32.inc();
	i32.dec();

	thefox::AtomicInt64 i64;
	i64.inc();
	i64.dec();

	return 0;
}

#include <stdio.h>
#include <base/thread.h>

using thefox::Thread;
using thefox::ThreadGroup;

void foo()
{
	printf("this is a thread function output\r\n");
}

int main(int argc, char **argv)
{
	printf("--- Thread ---\r\n");
	Thread t(&foo);
	t.start();
	t.join();

	printf("--- ThreadGroup ---\r\n");
	ThreadGroup tg;
	Thread t1(&foo);
	tg.addThread(&t1);
	tg.createThread(&foo);
	tg.createThread(&foo);
	tg.startAll();
	tg.joinAll();
	
	getchar();
	return 0;
}
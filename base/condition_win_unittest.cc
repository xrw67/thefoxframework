#include <stdio.h>
#include <base/thread.h>
#include <base/condition.h>

using namespace thefox;

Mutex *mutex;
Condition *cond;

void testThreadA()
{
	while (true) {
		Sleep(1000);
		cond->notify();
		cond->notify();
	}
}

void testThreadB()
{
	MutexLockGuard lock(*mutex);
	while (true) {
		cond->wait();
		printf("%u, cond->wait done\n", GetCurrentThreadId());
	}
}

int main(int argc, char **argv)
{
	mutex = new Mutex();
	cond = new Condition(*mutex);

	Thread a(testThreadA, "testThreadA");
	
	Thread b(testThreadB, "testThreadB");
	Thread c(testThreadB, "testThreadB");
	Thread d(testThreadB, "testThreadB");

	a.start();
	b.start();
	c.start();
	d.start();

	a.join();
	b.join();
	c.start();
	d.start();

	return 0;
}
#include <stdio.h>
#include <base/common.h>
#include <base/mem_pool.h>

using namespace thefox;

class TestA
{
	int32_t _usedBuf;
	char _buffer[4];
};

int main(int argc, char *argv[])
{
	MemPool<struct TestA> testaPool;

	TestA *ta = testaPool.get();
	TestA *tb = testaPool.get();
	TestA *tc = testaPool.get();
	TestA *td = testaPool.get();
	TestA *te = testaPool.get();
	TestA *tf = testaPool.get();
	TestA *tg = testaPool.get();
	TestA *th = testaPool.get();

	testaPool.put(ta);
	testaPool.put(tb);
	testaPool.put(tc);
	testaPool.put(td);
	testaPool.put(te);
	testaPool.put(tf);
	testaPool.put(tg);
	testaPool.put(th);

	ta = testaPool.get();
	tb = testaPool.get();
	tc = testaPool.get();
	td = testaPool.get();
	te = testaPool.get();
	tf = testaPool.get();
	tg = testaPool.get();
	th = testaPool.get();
	TestA *ti = testaPool.get();

	return 0;
}
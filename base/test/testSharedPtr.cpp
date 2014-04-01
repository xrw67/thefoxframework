#include <stdio.h>
#include <base/Types.h>
#include <base/Shareable.h>
#include <base/SharedPtr.h>

using namespace thefox;

class TestA : public Shareable
{
public:
	int32_t _usedBuf;
	char _buffer[4];
};

int main(int argc, char *argv[])
{
	SharedPtr<TestA> a;
	a = makeShared(new TestA());
	{
		SharedPtr<TestA> b = a;
		SharedPtr<TestA> c(b);
		c->_usedBuf = 2;
		strcpy(c->_buffer, "Hello");
	}
	return 0;
}
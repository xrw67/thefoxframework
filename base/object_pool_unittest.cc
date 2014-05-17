#include <base/object_pool.h>
#include <stdio.h>

using namespace thefox;

class TestA
{
public:
	TestA(const std::string &str)
		: _str(str) {}
	~TestA() {}
	std::string str() const { return _str; }
private:
	std::string _str;
};

int main(int argc, char *argv[])
{
	ObjectPool<TestA> pool;
	TestA *a = pool.get("hi");
	assert(NULL !=a);
	assert(a->str() == "hi");
	return 0;
}

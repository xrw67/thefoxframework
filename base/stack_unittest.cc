#include <stdio.h>
#include <base/stack.h>

int main(int argc, char **argv)
{
	thefox::Stack<int> s;
	s.push(3);
	s.push(5);
	int i;
	s.getTop(i);

	return 0;
}

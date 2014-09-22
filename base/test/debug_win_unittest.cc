#include "debug_win.h"

int main(int argc, char **argv)
{
	THEFOX_SET_UNHANDLED_EXCEPTION_FILTER;
	
	char *c = NULL;
	strcpy(c, "dfd"); // ²úÉúÖÐ¶Ï
	
	return 0;	
}
#include <stdext/posix/unistd.h>

// -------------------------------------------------------------------------

int main()
{
	truncate("/__test__.php", 5);
	return 0;
}

// -------------------------------------------------------------------------

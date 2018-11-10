#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"

int
main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf(2, "Please enter your requested PID ...\n");
		exit();
	}
	invoked_syscalls(atoi(argv[1]));
	exit();
}

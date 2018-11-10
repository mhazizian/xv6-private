#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"

int
main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf(2, "Please enter requested PID ...\n");
		exit();
	}
	sort_syscalls(atoi(argv[1]));
	exit();
}

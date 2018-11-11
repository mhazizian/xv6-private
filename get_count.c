#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"

int
main(int argc, char* argv[])
{
	sleep(3);
	get_count(getpid(), 13);
	exit();
}

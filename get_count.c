#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"

int
main(int argc, char* argv[])
{
	get_count(atoi(argv[1]), atoi(argv[2]));
	exit();
}

#include "types.h"
#include "user.h"

#define NCHILD 5

int main(int argc, char *argv[])
{
    int pid, queue;
    if (argc == 2 + 1)
	{
        pid = atoi(argv[1]);
        queue = atoi(argv[2]);
        changequeue(pid, queue);
		// printf(2, "pid:%d queu:%d\n", pid, queue);
		exit();
	}
	else
	{
		printf(2, "Invalid input!!!\n");
		exit();
	}
}

#include "types.h"
#include "user.h"
#include "ticketlock.h"

#define NCHILD 10

int main(int argc, char *argv[])
{
	int pid, i;
	struct ticketlock lock;
	ticketlockinit(&lock);

	pid = fork();
	for (i = 0; i < NCHILD; ++i)
		if (pid > 0)
			pid = fork();

	if (pid < 0)
	{
		printf(2, "fork error\n");
	}
	else if (pid == 0)
	{
//		printf(1, "child adding to shared counter\n");
		ticketlocktest(&lock);
	}
	else
	{
		for (int i = 0; i < NCHILD; ++i)
			wait();
//		printf(1, "user program finished\n");
	}

	exit();
}

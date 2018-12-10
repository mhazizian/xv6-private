#include "types.h"
#include "user.h"

#define NCHILD 2

int main(int argc, char *argv[])
{

	int pid, i;
//	struct ticketlock lock;
	rwinit_pw();

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
		// rwtest(36); // 100010
		rwtest_pw(21); // 10101
	}
	else
	{
		for (int i = 0; i < NCHILD + 1; ++i)
			wait();
//		printf(1, "user program finished\n");
	}

	exit();
}

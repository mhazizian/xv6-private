#include "types.h"
#include "user.h"

#define NCHILD 10

int main(int argc, char *argv[])
{

	int pid, i;
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
		pstat();
	}
	else
	{
		for (int i = 0; i < NCHILD + 1; ++i)
			wait();
	}

	exit();
}

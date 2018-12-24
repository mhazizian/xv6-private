#include "types.h"
#include "user.h"

#define NCHILD 5

int main(int argc, char *argv[])
{

	int pid, i;
	int temp;

	pid = fork();
	for (i = 0; i < NCHILD; ++i) {
		pid = fork();
	}

	if (pid < 0)
	{
		printf(2, "fork error\n");
	}

	for(i = 0; i < 1000000000 ; i++)
		temp = temp * 4;

	i = temp;



	if (pid > 0) {
		for (int i = 0; i < NCHILD + 1; ++i) {
			wait();
		}
		// changequeue(1, 2);
	}

	exit();
}
